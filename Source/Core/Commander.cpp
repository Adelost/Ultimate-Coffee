#include "stdafx.h"
#include "Commander.h"
#include "Command.h"
#include <fstream>
#include "Command_ChangeBackBufferColor.h"
#include "Command_TranslateSceneEntity.h"
#include <sys/stat.h> // struct stat
#include <memory> // memcpy
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

bool Commander::tryToAddCommandToHistoryAndExecute(Command* command)
{
	bool successfullyAdded = m_commandHistory->tryToAddCommand(command);
	if(successfullyAdded) // If the command was successfully added to the command history
	{
		command->doRedo(); // Execute command
	}
	return successfullyAdded;
}

bool Commander::tryToAddCommandToHistory(Command* command)
{
	return m_commandHistory->tryToAddCommand(command);
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
		Command* command = m_commandHistory->getNextCommandAndIncrementCurrent();
		command->doRedo();
		redoSucessful = true;
	}
	return redoSucessful;
}

bool Commander::tryToSaveCommandHistory(std::string path)
{
	std::ofstream outputFile(path, std::ios::binary);
	if(!outputFile.is_open())
	{
		return false;
	}
	int byteSize;
	char* byteData = m_commandHistory->receiveSerializedByteFormat(byteSize);

	outputFile.write(reinterpret_cast<const char*>(byteData), byteSize);
	outputFile.close();

	delete[] byteData;

	// check, remove if the command list gets very long and takes time to print
	// Prints the command history to the console in an effort to spot bugs (weird values in the printout)
	printCommandHistory();

	return true;
}

bool Commander::tryToLoadCommandHistory(std::string path)
{
	std::ifstream inputFile(path, std::ios::binary);
	if(!inputFile.is_open())
	{
		return false;
	}

	int bufferSize = 10000; // Standard size
	struct stat results;
	if(stat(path.c_str(), &results) == 0)
	{
		bufferSize = results.st_size; // size of file, if "struct stat" succeeded
	}
	else
	{
		MESSAGEBOX("Struct stat did not work. Inform Henrik. Mailto spidermine1@hotmail.com")
	}

	char* readData = new char[bufferSize];
	inputFile.read(readData, bufferSize);
	inputFile.close();

	//Reset GUI
	SEND_EVENT(&Event_RemoveCommandsFromCommandHistoryGUI(0, m_commandHistory->getNrOfCommands())); // Clear command history GUI list

	// Reset command history, before loading new command history data into it
	m_commandHistory->reset();

	bool result = m_commandHistory->tryToLoadFromSerializationByteFormat(readData, bufferSize);
	delete[] readData;

	if(result)
	{
		int GUI_Index = Converter::convertBetweenCommandHistoryIndexAndGUIListIndex(m_commandHistory->getIndexOfCurrentCommand(), m_commandHistory->getNrOfCommands());
		SEND_EVENT(&Event_SetSelectedCommandGUI(GUI_Index));

		// check, remove if the command list gets very long and takes time to print
		// Prints the command history to the console in an effort to spot bugs (weird values in the printout)
		printCommandHistory();
	}

	return result;
}

bool Commander::tryToJumpInCommandHistory(int jumpToIndex)
{
	int indexOfCurrentCommand = m_commandHistory->getIndexOfCurrentCommand();
	int nrOfCommands = m_commandHistory->getNrOfCommands();
	if(jumpToIndex < -1 || jumpToIndex >= nrOfCommands || nrOfCommands < 1)
	{
		return false; // Jump failed. Possible reasons: *Trying to jump to invalid index. *No commands in command history.
	}

	int nrOfCommandsInvolvedInJump = 0;
	if(indexOfCurrentCommand > -1) // Normal case: some command is current
	{
		if(jumpToIndex < indexOfCurrentCommand) // Backtrack by undoing commands
		{
			nrOfCommandsInvolvedInJump = indexOfCurrentCommand - jumpToIndex;
			m_commandHistory->backwardJump(nrOfCommandsInvolvedInJump);
		}
		else if(jumpToIndex > indexOfCurrentCommand) // Jump forward by redoing commands
		{
			nrOfCommandsInvolvedInJump = jumpToIndex - indexOfCurrentCommand;
			m_commandHistory->forwardJump(nrOfCommandsInvolvedInJump);
		}
		else if(jumpToIndex == indexOfCurrentCommand)
		{
			return true; // No jump needed. Treat jump as successful.
		}
	}
	else // Special case: no command is current, i.e. the current command is the command before the first command, i.e. no command is current.
	{
		nrOfCommandsInvolvedInJump = nrOfCommands - jumpToIndex;
		m_commandHistory->forwardJump(nrOfCommandsInvolvedInJump);
	}
}

void Commander::printCommandHistory()
{
	std::stringstream* commandHistoryAsText = m_commandHistory->getCommandHistoryAsText();
	std::cout << commandHistoryAsText->str() << std::endl;
	delete commandHistoryAsText;
}

int Commander::getCurrentCommandIndex()
{
	return m_commandHistory->getIndexOfCurrentCommand(); 
}

int Commander::getNrOfCommands()
{
	return m_commandHistory->getNrOfCommands();
}



CommandHistory::CommandHistory(void)
{
	reset();
}

CommandHistory::~CommandHistory(void)
{
	reset();
}

int CommandHistory::calculateSerializedByteSize()
{
	int serializedByteSize = 0;

	// Add size of the index of current command variable
	serializedByteSize += sizeof(m_indexOfCurrentCommand);

	// Add size of commands
	int nrOfCommands = m_commands.size();
	for(int i=0;i<nrOfCommands;i++)
	{
		Command* command = m_commands.at(i);
		serializedByteSize += command->getByteSizeOfDataStruct(); // Add size of command data struct
		serializedByteSize += sizeof(command->getType()); // Add size of command type variable
	}

	return serializedByteSize;
}

void CommandHistory::setCurrentCommand(int index)
{
	int nrOfCommands = m_commands.size();
	if(index > -2 && index < nrOfCommands) //bounds checking
	{
		m_indexOfCurrentCommand = index;
	}
}

bool CommandHistory::tryToAddCommand(Command* command)
{
	//--------------------------------------------------------------------------------------
	// Prevents uninitialized commands from being added by verifying the "command" pointer
	//--------------------------------------------------------------------------------------
	// Note: this safeguard only works if the command is set to NULL or nullptr, i.e. the safeguard is useless. Do not rely on it.
	if(command==NULL || command==nullptr)
	{
		return false;
	}
	// Extra safeguard:
	command->getType(); //If the program crashes here, it means that "command" is uninitialized, which it should not be below this line.
	
	//--------------------------------------------------------------------------------------
	// Add command
	//--------------------------------------------------------------------------------------
	int nrOfCommands = m_commands.size();
	if(m_indexOfCurrentCommand == nrOfCommands-1) //expand vector
	{
		m_commands.push_back(command);
		setCurrentCommand(m_indexOfCurrentCommand+1);
	}
	else // overwrite old command and forget about history after this point
	{
		setCurrentCommand(m_indexOfCurrentCommand+1);
		Command* oldCommand = m_commands.at(m_indexOfCurrentCommand);
		delete oldCommand;
		m_commands.at(m_indexOfCurrentCommand) = command;

		int newSize = (m_indexOfCurrentCommand+1);
		int nrOfCommandsLeftToBeRemoved = nrOfCommands - newSize;
		for(int i=0;i<nrOfCommandsLeftToBeRemoved;i++)
		{
			Command* removedCommand = m_commands.at(newSize+i);
			delete removedCommand;
		}
		m_commands.resize(newSize);
		int GUI_Index = Converter::convertBetweenCommandHistoryIndexAndGUIListIndex(m_indexOfCurrentCommand, getNrOfCommands());
		SEND_EVENT(&Event_RemoveCommandsFromCommandHistoryGUI(GUI_Index, nrOfCommandsLeftToBeRemoved+1));
	}

	return true;
}

Command* CommandHistory::getCurrentCommandAndDecrementCurrent()
{
	Command* command = m_commands.at(m_indexOfCurrentCommand);
	setCurrentCommand(m_indexOfCurrentCommand-1);
	return command;
}

Command* CommandHistory::getNextCommandAndIncrementCurrent()
{
	setCurrentCommand(m_indexOfCurrentCommand+1);
	Command* command = m_commands.at(m_indexOfCurrentCommand);
	return command;
}

void CommandHistory::forwardJump(int nrOfSteps)
{
	for(int i=0;i<nrOfSteps;i++)
	{
		Command* command = getNextCommandAndIncrementCurrent();
		command->doRedo();
	}
}

void CommandHistory::backwardJump(int nrOfSteps)
{
	for(int i=0;i<nrOfSteps;i++)
	{
		Command* command = getCurrentCommandAndDecrementCurrent();
		command->undo();
	}
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

char* CommandHistory::receiveSerializedByteFormat(int& byteSize)
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
	byteSize = calculateSerializedByteSize();
	char* byteData = new char[byteSize];
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

bool CommandHistory::tryToLoadFromSerializationByteFormat(char* bytes, int byteSize)
{
	// Refer to "receiveSerializedByteFormat" for format description
	// The byte array "bytes" is navigated using the index "nextByte"
	int nextByte = 0;

	// First, load index of current command
	int loadedIndexOfCurrentCommand= *reinterpret_cast<int*>(bytes+nextByte);
	nextByte += sizeof(loadedIndexOfCurrentCommand);

	int sizeOfTypeVarible = sizeof(Enum::CommandType);
	while(nextByte < byteSize)
	{
		// Interpret command type and command data struct from the byte array "bytes"
		Command* command = NULL;
		Enum::CommandType commandType = *reinterpret_cast<Enum::CommandType*>(bytes+nextByte);
		nextByte += sizeOfTypeVarible;
		char* commandDataStructBytes = reinterpret_cast<char*>(bytes+nextByte);

		switch(commandType) // Create command according to its type
		{
		case Enum::CommandType::TRANSLATE_SCENE_ENTITY:
			{
				command = new Command_TranslateSceneEntity();
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
			if(!tryToAddCommand(command))
			{
				return false;
			}
			if(m_indexOfCurrentCommand <= loadedIndexOfCurrentCommand)
			{
				command->doRedo(); // Execute command up and until current, as loaded from file
			}
			SEND_EVENT(&Event_AddCommandToCommandHistoryGUI(command)); //Add all commands to GUI

			nextByte += command->getByteSizeOfDataStruct();
		}
		else
		{
			return false;
		}
	}

	setCurrentCommand(loadedIndexOfCurrentCommand);

	return true;
}

std::stringstream* CommandHistory::getCommandHistoryAsText()
{
	int nrOfCommands = m_commands.size();
	std::stringstream* text = new std::stringstream();

	*text << "\n-----Printout of command history------\n";
	*text << "Byte size: " << calculateSerializedByteSize() << "\n";
	*text << "Number of commands: " << nrOfCommands << "\n";
	*text << "Index of current command: " << m_indexOfCurrentCommand << "\n";
	*text << "Format of command list below: index, type, byte size\n";
	for(int i=0;i<nrOfCommands;i++)
	{
		Command* command = m_commands.at(i);
		int type = command->getType();
		int size = command->getByteSizeOfDataStruct();
		*text << i << ", " << type << ", " << size << "\n";
	}
	*text << "--------------------------------------\n";
	return text;
}

void CommandHistory::reset()
{
	int nrOfCommands = m_commands.size();
	for(int i=0;i<nrOfCommands;i++)
	{
		Command* command = m_commands.at(i);
		delete command;
	}
	m_commands.clear();
	m_indexOfCurrentCommand = -1;
}