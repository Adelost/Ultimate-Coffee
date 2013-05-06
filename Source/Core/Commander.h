#pragma once

#include <vector>

class CommandHistory;
class Command;

//--------------------------------------------------------------------------------------
// Manipulates "Command"s in a "CommandHistory".
//
// "Command"s can be:
// *Stored (tryToAddCommandToHistory)
// *Executed (tryToAddCommandToHistoryAndExecute)
// *Undone (tryToUndoLatestCommand)
// *Saved to file (tryToSaveCommandHistory)
// *Loaded from file (tryToLoadCommandHistory)
// *Printed to std::cout (printCommandHistory)
//--------------------------------------------------------------------------------------
class Commander
{
private:
	CommandHistory* m_commandHistory;

public:
	Commander(void);
	~Commander(void);
	
	// Initialilzes the command history. Success status is returned.
	bool init();
	
	// Possible to use as a visual cue
	bool redoIsPossible();

	// Possible to use as a visual cue
	bool undoIsPossible();
	
	// Stores "command" in the command history, and executes it, if it was successfully added. Returns true if "command" was sucessfully added, otherwise false.
	bool tryToAddCommandToHistoryAndExecute(Command* command);

	// Stores "command" in the command history, without executing it. Returns true if successfully added, otherwise false.
	bool tryToAddCommandToHistory(Command* command);

	// Returns true if undo was successful, otherwise false
	bool tryToUndoLatestCommand();

	// Returns true if redo was successful, otherwise false
	bool tryToRedoLatestUndoCommand();

	// Returns true if the command history was successfully saved to file given by "path", otherwise false
	bool tryToSaveCommandHistory(std::string path);

	// Returns true if a command history was successfully loaded from file given by "path", otherwise false
	bool tryToLoadCommandHistory(std::string path);

	// Backtracks by undoing all commands from current until index is reached, OR track forward by redoing alla commands from current until index is reached
	void trackToIndex(int index);

	// Prints command list to "std::cout"
	void printCommandHistory();

	int getCurrentCommandIndex();
	int getNrOfCommands();
};


//--------------------------------------------------------------------------------------
// Encapsulates a vector of "Command"s and an index identifying the current command.
//--------------------------------------------------------------------------------------
class CommandHistory
{
private:
	std::vector<Command*> m_commands;		// Stores all commands that the "CommandHistory" is responsible for
	int m_indexOfCurrentCommand;			// -1 means that no command is current

	void setCurrentCommand(int index);
	int calculateSerializedByteSize();

public:
	CommandHistory(void);
	~CommandHistory(void);
	
	// Returns true if succeeded, otherwise false
	bool tryToAddCommand(Command* command);
	
	Command* getCurrentCommandAndDecrementCurrent();
	Command* incrementCurrentAndGetCurrentCommand();
	
	bool thereExistsCommandsAfterCurrentCommand();
	bool thereExistsCommandsBeforeCurrentCommand();
	
	// "byteSize" is a return value. Refer to .cpp file for format description.
	char* receiveSerializedByteFormat(int& byteSize); 

	// "bytes" is assumed to be in the serialized byte format as returned from "receiveSerializedByteFormat". "byteSize" is assumed to be the size in bytes of the "bytes" parameter.
	bool tryToLoadFromSerializationByteFormat(char* bytes, int byteSize);

	// Returns the command history information as text
	std::stringstream* getCommandHistoryAsText();

	void executeAllCommandsUpAndUntilCurrent();

	void reset();

	int getIndexOfCurrentCommand(){return m_indexOfCurrentCommand;}
	int getNrOfCommands(){return m_commands.size();}
};