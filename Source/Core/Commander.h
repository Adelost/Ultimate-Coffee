#pragma once
#include <vector>

class CommandHistory;
class Command;

//--------------------------------------------------------------------------------------
// Manipulates "Command"s in a "CommandHistory".
//--------------------------------------------------------------------------------------
class Commander
{
private:
	CommandHistory* commandHistory_;

public:
	Commander(void);
	~Commander(void);
	
	bool init();
	
	bool redoIsPossible();
	bool undoIsPossible();
	
	void addToHistoryAndExecute(Command* command);		//Executes "Command" and stores it in the command history
	void addToHistory(Command* command);				//Stores "Command" in the command history, without executing it
	
	bool tryToUndoLatestCommand();						//Returns true if undo was sucessful, otherwise false
	bool tryToRedoLatestUndoCommand();					//Returns true if redo was sucessful, otherwise false
	
	bool tryToSaveCommandHistory(std::string path);		//Returns true if the command history was sucessfully saved, otherwise false
	bool tryToLoadCommandHistory(std::string path);		//Returns true if a command history was sucessfully loaded, otherwise false

	void displayCommandList();
};


//--------------------------------------------------------------------------------------
// Encapsulates a vector of "Command"s and an index identifying the current command.
//--------------------------------------------------------------------------------------
class CommandHistory
{
private:
	std::vector<Command*> commands_;		//Stores all commands that the "CommandHistory" is responsible for
	int indexOfCurrentCommand_;				//-1 means that no command is current

	void setCurrentCommand(int index);
	int calculateSerializedByteSize();

public:
	CommandHistory(void);
	~CommandHistory(void);
	
	void addCommand(Command* command);
	
	Command* getCurrentCommandAndDecrementCurrent();
	Command* incrementCurrentAndGetCurrentCommand();
	
	bool thereExistsCommandsAfterCurrentCommand();
	bool thereExistsCommandsBeforeCurrentCommand();
	
	char* receiveSerializedByteFormat(int& byteSize); //"byteSize" is a return value
	bool loadFromSerializationByteFormat(char* bytes, int byteSize);

	std::string* getCommandList(int& nrOfCommands); //"nrOfCommands" is a return value

	void executeAllCommandsUpAndUntilCurrent();
};