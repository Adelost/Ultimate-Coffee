#include "Commander.h"
#include "Command.h"
#include <fstream>
#include "Command_ChangeBackBufferColor.h"
#include <sys/stat.h> //struct stat
#include <iostream> //console output
#include <memory> //memcpy

Commander::Commander(void)
{
	commandHistory_ = NULL;
}

Commander::~Commander(void)
{
	delete commandHistory_;
}

bool Commander::init()
{
	commandHistory_ = new CommandHistory();
	return true;
}

void Commander::update()
{
	if(commandHistory_->thereExistsCommandsAfterCurrentCommand())
	{
		//redo is possible
		//check, add code to inform GUI
	}
	if(commandHistory_->thereExistsCommandsBeforeCurrentCommand())
	{
		//undo is possible
		//check, add code to inform GUI
	}
}

void Commander::addToHistoryAndExecute(Command* command)
{
	command->doRedo();
	commandHistory_->addCommand(command);
}

void Commander::addToHistory(Command* command)
{
	commandHistory_->addCommand(command);
}

bool Commander::tryToUndoLatestCommand()
{
	bool undoSucessful = false;
	if(commandHistory_->thereExistsCommandsBeforeCurrentCommand())
	{
		Command* command = commandHistory_->getCurrentCommandAndDecrementCurrent();
		command->undo();
		undoSucessful = true;
	}
	return undoSucessful;
}

bool Commander::tryToRedoLatestUndoCommand()
{
	bool redoSucessful = false;
	if(commandHistory_->thereExistsCommandsAfterCurrentCommand())
	{
		Command* command = commandHistory_->incrementCurrentAndGetCurrentCommand();
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
	char* byteData = commandHistory_->receiveSerializedByteFormat(byteSize);

	outputFile.write(reinterpret_cast<const char*>(byteData), byteSize);
	outputFile.close();

	delete[] byteData;

	return true;
}

bool Commander::tryToLoadCommandHistory(std::string path)
{
	std::ifstream inputFile(path, std::ios::binary);
	if(!inputFile.is_open())
	{
		return false;
	}

	int bufferSize = 10000; //Standard size
	struct stat results;
	if(stat(path.c_str(), &results) == 0)
	{
		bufferSize = results.st_size; //size of file, if "struct stat" succeeded
	}
	else
	{
		//check, notify that "struct stat" did not work
		//hope that a bufferSize of 10000 is enough
	}

	char* readData = new char[bufferSize];
	inputFile.read(readData, bufferSize);
	inputFile.close();

	delete commandHistory_;
	commandHistory_ = new CommandHistory();
	bool result = commandHistory_->loadFromSerializationByteFormat(readData, bufferSize);
	delete[] readData;

	commandHistory_->executeAllCommandsUpAndUntilCurrent();

	return result;
}

void Commander::displayCommandList()
{
	int nrOfCommands = 0;
	std::string* commandList = commandHistory_->getCommandList(nrOfCommands);

	for(int i=0;i<nrOfCommands;i++)
	{
		std::string str = commandList[i]; 
		std::cout << str.c_str() << std::endl;
	}

	delete[] commandList;
}


CommandHistory::CommandHistory(void)
{
	indexOfCurrentCommand_ = -1;
}

CommandHistory::~CommandHistory(void)
{
	int nrOfCommands = commands_.size();
	for(int i=0;i<nrOfCommands;i++)
	{
		Command* command = commands_.at(i);
		delete command;
	}
	indexOfCurrentCommand_ = -1;
}

int CommandHistory::calculateSerializedByteSize()
{
	int serializedByteSize = 0;
	int nrOfCommands = commands_.size();
	for(int i=0;i<nrOfCommands;i++)
	{
		Command* command = commands_.at(i);
		serializedByteSize += command->getByteSizeOfDataStruct();
		serializedByteSize += sizeof(command->getType());
	}
	return serializedByteSize;
}

void CommandHistory::setCurrentCommand(int index)
{
	int nrOfCommands = commands_.size();
	if(index > -2 && index < nrOfCommands) //bounds checking
	{
		indexOfCurrentCommand_ = index;
	}
}

void CommandHistory::addCommand(Command* command)
{
	int nrOfCommands = commands_.size();
	if(indexOfCurrentCommand_ == nrOfCommands-1) //expand vector
	{
		commands_.push_back(command);
		setCurrentCommand(indexOfCurrentCommand_+1);
	}
	else //overwrite old command and forget about history after this point
	{
		setCurrentCommand(indexOfCurrentCommand_+1);
		Command* oldCommand = commands_.at(indexOfCurrentCommand_);
		delete oldCommand;
		commands_.at(indexOfCurrentCommand_) = command;

		int newSize = (indexOfCurrentCommand_+1);
		int nrOfRemovedCommands = nrOfCommands - newSize;
		for(int i=0;i<nrOfRemovedCommands;i++)
		{
			Command* removedCommand = commands_.at(newSize+i);
			delete removedCommand;
		}
		commands_.resize(newSize);
	}
}

Command* CommandHistory::getCurrentCommandAndDecrementCurrent()
{
	Command* command = commands_.at(indexOfCurrentCommand_);
	setCurrentCommand(indexOfCurrentCommand_-1);
	return command;
}

Command* CommandHistory::incrementCurrentAndGetCurrentCommand()
{
	setCurrentCommand(indexOfCurrentCommand_+1);
	Command* command = commands_.at(indexOfCurrentCommand_);
	return command;
}

bool CommandHistory::thereExistsCommandsAfterCurrentCommand()
{
	int nrOfCommands = commands_.size();
	int differance = abs(indexOfCurrentCommand_-nrOfCommands);
	return (differance > 1 && nrOfCommands > 0);
}

bool CommandHistory::thereExistsCommandsBeforeCurrentCommand()
{
	return (indexOfCurrentCommand_ > -1);
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
	int sizeOfIndexOfCurrentCommandVariable = sizeof(indexOfCurrentCommand_);
	byteSize = calculateSerializedByteSize() + sizeOfIndexOfCurrentCommandVariable;
	char* byteData = new char[byteSize];
	int writeNextByteToThisIndex = 0;

	//First, insert "indexOfCurrentCommand_" into the byte array
	memcpy(byteData+writeNextByteToThisIndex, &indexOfCurrentCommand_, sizeOfIndexOfCurrentCommandVariable); //Copy "indexOfCurrentCommand_" into the byte array
	writeNextByteToThisIndex += sizeOfIndexOfCurrentCommandVariable; //Increment byte array index counter

	//Then, insert data of all "Command"s into the byte array
	int nrOfCommands = commands_.size();
	for(int i=0;i<nrOfCommands;i++)
	{
		Command* command = commands_.at(i);
		//Insert command data into byte array
		command->receiveDataStructInSerializationFormat(byteData, writeNextByteToThisIndex);
	}

	return byteData;
}

bool CommandHistory::loadFromSerializationByteFormat(char* bytes, int byteSize)
{
	//Refer to "receiveSerializedByteFormat" for format description
	//The byte array "bytes" is navigated using the index "nextByte"
	int nextByte = 0;

	//First, load index of current command
	int indexOfCurrentCommand_ = *reinterpret_cast<int*>(bytes+nextByte);
	nextByte += sizeof(indexOfCurrentCommand_);

	int sizeOfTypeVarible = sizeof(Enum::CommandType);
	while(nextByte < byteSize)
	{
		//Interpret command type and command data struct from the byte array "bytes"
		Command* command = NULL;
		Enum::CommandType commandType = *reinterpret_cast<Enum::CommandType*>(bytes+nextByte);
		nextByte += sizeOfTypeVarible;
		char* commandDataStructBytes = reinterpret_cast<char*>(bytes+nextByte);

		switch(commandType) //Create command according to its type
		{
		case Enum::CommandType::TRANSLATE:
			{
				//command = new Command_Translate();
				break;
			}
		case Enum::CommandType::ROTATE:
			{
				//command = new Command_Rotate();
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
		if(command!=NULL) //Load command data, as interpreted from the byte array, and add the command to the command history
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

std::string* CommandHistory::getCommandList(int& nrOfCommands)
{
	nrOfCommands = commands_.size();
	std::string* listOfCommands = new std::string[nrOfCommands];
	std::string currentCommand = "Undefined";
	for(int i=0;i<nrOfCommands;i++)
	{
		Enum::CommandType commandType = commands_.at(i)->getType();
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
	int nrOfCommands = commands_.size();
	for(int i=0;i<indexOfCurrentCommand_;i++)
	{
		Command* command = commands_.at(i);
		command->doRedo();
	}
}