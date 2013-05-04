#pragma once

//#include <vector>

class CommandHistory;
class Command;

//--------------------------------------------------------------------------------------
// Manipulates "Command"s in a "CommandHistory".
//--------------------------------------------------------------------------------------
class Commander
{
private:
	CommandHistory* m_commandHistory;

public:
	Commander(void);
	~Commander(void);
	
	bool init();
	
	bool redoIsPossible();
	bool undoIsPossible();
	
	void addToHistoryAndExecute(Command* p_command);	// Executes "Command" and stores it in the command history
	void addToHistory(Command* p_command);				// Stores "Command" in the command history, without executing it

	bool tryToUndoLatestCommand();						// Returns true if undo was successful, otherwise false
	bool tryToRedoLatestUndoCommand();					// Returns true if redo was successful, otherwise false

	bool tryToSaveCommandHistory(std::string p_path);	// Returns true if the command history was successfully saved, otherwise false
	bool tryToLoadCommandHistory(std::string p_path);	// Returns true if a command history was successfully loaded, otherwise false

	void displayCommandList();
};


//--------------------------------------------------------------------------------------
// Encapsulates a vector of "Command"s and an index identifying the current command.
//--------------------------------------------------------------------------------------
class CommandHistory
{
private:
	std::vector<Command*> m_commands;		// Stores all commands that the "CommandHistory" is responsible for
	int m_indexOfCurrentCommand;			// -1 means that no command is current

	void setCurrentCommand(int p_index);
	int calculateSerializedByteSize();

public:
	CommandHistory(void);
	~CommandHistory(void);
	
	void addCommand(Command* p_command);
	
	Command* getCurrentCommandAndDecrementCurrent();
	Command* incrementCurrentAndGetCurrentCommand();
	
	bool thereExistsCommandsAfterCurrentCommand();
	bool thereExistsCommandsBeforeCurrentCommand();
	
	char* receiveSerializedByteFormat(int& p_byteSize); // "byteSize" is a return value
	bool loadFromSerializationByteFormat(char* p_bytes, int p_byteSize);

	std::string* getCommandList(int& p_nrOfCommands);	// "nrOfCommands" is a return value

	void executeAllCommandsUpAndUntilCurrent();
};