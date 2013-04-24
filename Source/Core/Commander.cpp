#include "Commander.h"
#include "Command.h"
#include <fstream>
#include "Command_ChangeBackBufferColor.h"
#include <sys/stat.h> // struct stat
#include <iostream> // console output
#include <memory> // memcpy

#include "EventManager.h"
#include "Events.h"

Commander::Commander(void)
{
	m_commandHistory = NULL;
}

Commander::~Commander(void)
{
	delete m_commandHistory;
}

bool Commander::init()
{
	m_commandHistory = new CommandHistory();
	return true;
}

bool Commander::undoIsPossible()
{
	return m_commandHistory->thereExistsCommandsBeforeCurrentCommand();
}

bool Commander::redoIsPossible()
{
	return m_commandHistory->thereExistsCommandsAfterCurrentCommand();
}

void Commander::addToHistoryAndExecute( Command* p_command )
{
	p_command->doRedo();
	m_commandHistory->addCommand(p_command);
}

void Commander::addToHistory( Command* p_command )
{
	m_commandHistory->addCommand(p_command);
}

bool Commander::tryToUndoLatestCommand()
{
	bool undoSucessful = false;
	if(m_commandHistory->thereExistsCommandsBeforeCurrentCommand())
	{
		Command* command = m_commandHistory->getCurrentCommandAndDecrementCurrent();
		command->undo();
		undoSucessful = true;
	}
	return undoSucessful;
}

bool Commander::tryToRedoLatestUndoCommand()
{
	bool redoSucessful = false;
	if(m_commandHistory->thereExistsCommandsAfterCurrentCommand())
	{
		Command* command = m_commandHistory->incrementCurrentAndGetCurrentCommand();
		command->doRedo();
		redoSucessful = true;
	}
	return redoSucessful;
}

bool Commander::tryToSaveCommandHistory( std::string p_path )
{
	std::ofstream outputFile(p_path, std::ios::binary);
	if(!outputFile.is_open())
	{
		return false;
	}
	int byteSize;
	char* byteData = m_commandHistory->receiveSerializedByteFormat(byteSize);

	outputFile.write(reinterpret_cast<const char*>(byteData), byteSize);
	outputFile.close();

	delete[] byteData;

	return true;
}

bool Commander::tryToLoadCommandHistory( std::string p_path )
{
	std::ifstream inputFile(p_path, std::ios::binary);
	if(!inputFile.is_open())
	{
		return false;
	}

	int bufferSize = 10000; // Standard size
	struct stat results;
	if(stat(p_path.c_str(), &results) == 0)
	{
		bufferSize = results.st_size; // size of file, if "struct stat" succeeded
	}
	else
	{
		MESSAGEBOX("Struct stat did not work. Inform Henrik.")
	}

	char* readData = new char[bufferSize];
	inputFile.read(readData, bufferSize);
	inputFile.close();

	delete m_commandHistory;
	m_commandHistory = new CommandHistory();
	bool result = m_commandHistory->loadFromSerializationByteFormat(readData, bufferSize);
	delete[] readData;

	m_commandHistory->executeAllCommandsUpAndUntilCurrent();

	return result;
}

void Commander::displayCommandList()
{
	int nrOfCommands = 0;
	std::string* commandList = m_commandHistory->getCommandList(nrOfCommands);

	for(int i=0;i<nrOfCommands;i++)
	{
		std::string str = commandList[i]; 
		std::cout << str.c_str() << std::endl;
	}

	delete[] commandList;
}


CommandHistory::CommandHistory(void)
{
	m_indexOfCurrentCommand = -1;
}

CommandHistory::~CommandHistory(void)
{
	int nrOfCommands = m_commands.size();
	for(int i=0;i<nrOfCommands;i++)
	{
		Command* command = m_commands.at(i);
		delete command;
	}
	m_indexOfCurrentCommand = -1;
}

int CommandHistory::calculateSerializedByteSize()
{
	int serializedByteSize = 0;
	int nrOfCommands = m_commands.size();
	for(int i=0;i<nrOfCommands;i++)
	{
		Command* command = m_commands.at(i);
		serializedByteSize += command->getByteSizeOfDataStruct();
		serializedByteSize += sizeof(command->getType());
	}
	return serializedByteSize;
}

void CommandHistory::setCurrentCommand( int p_index )
{
	int nrOfCommands = m_commands.size();
	if(p_index > -2 && p_index < nrOfCommands) //bounds checking
	{
		m_indexOfCurrentCommand = p_index;
	}
}

void CommandHistory::addCommand( Command* p_command )
{
	int nrOfCommands = m_commands.size();
	if(m_indexOfCurrentCommand == nrOfCommands-1) //expand vector
	{
		m_commands.push_back(p_command);
		setCurrentCommand(m_indexOfCurrentCommand+1);
	}
	else // overwrite old command and forget about history after this point
	{
		setCurrentCommand(m_indexOfCurrentCommand+1);
		Command* oldCommand = m_commands.at(m_indexOfCurrentCommand);
		delete oldCommand;
		m_commands.at(m_indexOfCurrentCommand) = p_command;

		int newSize = (m_indexOfCurrentCommand+1);
		int nrOfRemovedCommands = nrOfCommands - newSize;
		for(int i=0;i<nrOfRemovedCommands;i++)
		{
			Command* removedCommand = m_commands.at(newSize+i);
			delete removedCommand;
		}
		m_commands.resize(newSize);
	}
}

Command* CommandHistory::getCurrentCommandAndDecrementCurrent()
{
	Command* command = m_commands.at(m_indexOfCurrentCommand);
	setCurrentCommand(m_indexOfCurrentCommand-1);
	return command;
}

Command* CommandHistory::incrementCurrentAndGetCurrentCommand()
{
	setCurrentCommand(m_indexOfCurrentCommand+1);
	Command* command = m_commands.at(m_indexOfCurrentCommand);
	return command;
}

bool CommandHistory::thereExistsCommandsAfterCurrentCommand()
{
	int nrOfCommands = m_commands.size();
	int differance = abs(m_indexOfCurrentCommand-nrOfCommands);
	return (differance > 1 && nrOfCommands > 0);
}

bool CommandHistory::thereExistsCommandsBeforeCurrentCommand()
{
	return (m_indexOfCurrentCommand > -1);
}

char* CommandHistory::receiveSerializedByteFormat( int& p_byteSize )
{
	//--------------------------------------------------------------------------------------
	// Format description: the index of the current command comes first,
	// followed by each "Command", which consist of a type and a data struct.
	// Illustrating example of format:
	// 2
	// 0
	// {data from a translate command struct}
	// 0
	// {data from a translate command struct}
	// 1
	// {data from a rotate command struct}
	//--------------------------------------------------------------------------------------
	int sizeOfIndexOfCurrentCommandVariable = sizeof(m_indexOfCurrentCommand);
	p_byteSize = calculateSerializedByteSize() + sizeOfIndexOfCurrentCommandVariable;
	char* byteData = new char[p_byteSize];
	int writeNextByteToThisIndex = 0;

	// First, insert "indexOfCurrentCommand_" into the byte array
	memcpy(byteData+writeNextByteToThisIndex, &m_indexOfCurrentCommand, sizeOfIndexOfCurrentCommandVariable); //Copy "indexOfCurrentCommand_" into the byte array
	writeNextByteToThisIndex += sizeOfIndexOfCurrentCommandVariable; //Increment byte array index counter

	// Then, insert data of all "Command"s into the byte array
	int nrOfCommands = m_commands.size();
	for(int i=0;i<nrOfCommands;i++)
	{
		Command* command = m_commands.at(i);
		// Insert command data into byte array
		command->receiveDataStructInSerializationFormat(byteData, writeNextByteToThisIndex);
	}

	return byteData;
}

bool CommandHistory::loadFromSerializationByteFormat( char* p_bytes, int p_byteSize )
{
	// Refer to "receiveSerializedByteFormat" for format description
	// The byte array "bytes" is navigated using the index "nextByte"
	int nextByte = 0;

	// First, load index of current command
	int indexOfCurrentCommand_ = *reinterpret_cast<int*>(p_bytes+nextByte);
	nextByte += sizeof(indexOfCurrentCommand_);

	int sizeOfTypeVarible = sizeof(Enum::CommandType);
	while(nextByte < p_byteSize)
	{
		// Interpret command type and command data struct from the byte array "bytes"
		Command* command = NULL;
		Enum::CommandType commandType = *reinterpret_cast<Enum::CommandType*>(p_bytes+nextByte);
		nextByte += sizeOfTypeVarible;
		char* commandDataStructBytes = reinterpret_cast<char*>(p_bytes+nextByte);

		switch(commandType) // Create command according to its type
		{
		case Enum::CommandType::TRANSLATE:
			{
				// command = new Command_Translate();
				break;
			}
		case Enum::CommandType::ROTATE:
			{
				// command = new Command_Rotate();
				break;
			}
		case Enum::CommandType::CHANGEBACKBUFFERCOLOR:
			{
				command = new Command_ChangeBackBufferColor();
				break;
			}
		default:
			{
				return false;
				break;
			}
		}
		if(command!=NULL) // Load command data, as interpreted from the byte array, and add the command to the command history
		{
			command->loadDataStructFromBytes(commandDataStructBytes);
			addCommand(command);
			nextByte += command->getByteSizeOfDataStruct();
		}
		else
		{
			return false;
		}
	}

	setCurrentCommand(indexOfCurrentCommand_);

	return true;
}

std::string* CommandHistory::getCommandList( int& p_nrOfCommands )
{
	p_nrOfCommands = m_commands.size();
	std::string* listOfCommands = new std::string[p_nrOfCommands];
	std::string currentCommand = "Undefined";
	for(int i=0;i<p_nrOfCommands;i++)
	{
		Enum::CommandType commandType = m_commands.at(i)->getType();
		switch(commandType)
		{
		case Enum::CommandType::TRANSLATE:
			currentCommand = "Translate";
			break;
		case Enum::CommandType::ROTATE:
			currentCommand = "Rotate";
			break;
		case Enum::CommandType::CHANGEBACKBUFFERCOLOR:
			currentCommand = "Backbuffer color";
			break;
		}
		listOfCommands[i] = currentCommand;
	}
	return listOfCommands;
}

void CommandHistory::executeAllCommandsUpAndUntilCurrent()
{
	int nrOfCommands = m_commands.size();
	for(int i=0;i<m_indexOfCurrentCommand;i++)
	{
		Command* command = m_commands.at(i);
		command->doRedo();
	}
}