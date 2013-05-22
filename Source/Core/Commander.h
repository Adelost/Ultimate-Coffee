#pragma once

#include <vector>

class CommandHistory;
class Command;

//--------------------------------------------------------------------------------------
// Manages a "CommandHistory" of "Command"s
//
// Functionality:
// *Store command (tryToAddCommandToHistory)
// *Store and execute command (tryToAddCommandToHistoryAndExecute)
// *Save command history to file (tryToSaveCommandHistory)
// *Load command history from file (tryToLoadCommandHistory)
// *Print command history text information to std::cout or to file (printCommandHistory)
// *Jump to certain index in command history (tryToJumpInCommandHistory)
//--------------------------------------------------------------------------------------
class Commander
{
	friend class CommanderSpy;
private:
	CommandHistory* m_commandHistory;

public:
	Commander(void);
	~Commander(void);
	
	// Initialilzes the command history. Success status is returned.
	bool init();

	// Stores "command" in the command history, and executes it, if it was successfully added. Returns true if "command" was successfully added, otherwise false.
	bool tryToAddCommandToHistoryAndExecute(Command* command);

	// Stores "command" in the command history, without executing it. Returns true if successfully added, otherwise false.
	bool tryToAddCommandToHistory(Command* command);

	// Returns true if the command history was successfully saved to file given by "path", otherwise false
	bool tryToSaveCommandHistory(std::string path);

	// Returns true if a command history was successfully loaded from file given by "path", otherwise false
	bool tryToLoadCommandHistory(std::string path);

	// Jumps to "jumpToIndex" in command history. Returns true if successful, otherwise false.
	bool tryToJumpInCommandHistory(int jumpToIndex);

	// Prints command list to "std::cout"
	void printCommandHistory();

	// Print command list to file at "path"
	void printCommandHistory(std::string path);

	void reset();
	int getCurrentCommandIndex();
	int getNrOfCommands();
};


//--------------------------------------------------------------------------------------
// Encapsulates a vector of "Command"s and an index identifying the current command.
// Functionality:
// *Add command (tryToAddCommand)
// *Load from byte format (tryToLoadFromSerializationByteFormat)
// *Save to byte format (receiveSerializedByteFormat)
// *Retrieve as std::stringstream (getCommandHistoryAsText)
// *History jumping (tryToJumpInCommandHistory)
//--------------------------------------------------------------------------------------
class CommandHistory
{
	friend class CommanderSpy;
private:
	std::vector<Command*> m_commands;		// Stores all commands that the "CommandHistory" is responsible for
	int m_indexOfCurrentCommand;			// -1 means that no command is current

	void setCurrentCommand(int index);
	int calculateSerializedByteSize();

	// Tracks forward by redoing "nrOfSteps" "Command"s from current
	void forwardJump(int nrOfSteps);

	// Backtracks by undoing "nrOfSteps" "Command"s from current
	void backwardJump(int nrOfSteps);

public:
	CommandHistory(void);
	~CommandHistory(void);
	
	// Returns true if succeeded, otherwise false
	bool tryToAddCommand(Command* command);
	
	// Backtracks by undoing all commands from current until index is reached, OR track forward by redoing all commands from current until index is reached. Returns true if jump was successful, otherwise false.
	bool tryToJumpInCommandHistory(int jumpToIndex);

	// "byteSize" is a return value. Refer to .cpp file for format description.
	char* receiveSerializedByteFormat(int& byteSize); 

	// "bytes" is assumed to be in the serialized byte format as returned from "receiveSerializedByteFormat". "byteSize" is assumed to be the size in bytes of the "bytes" parameter.
	bool tryToLoadFromSerializationByteFormat(char* bytes, int byteSize);

	// Returns the command history information as text
	std::stringstream* getCommandHistoryAsText();

	void reset();

	int getIndexOfCurrentCommand(){return m_indexOfCurrentCommand;}
	int getNrOfCommands(){return m_commands.size();}
};