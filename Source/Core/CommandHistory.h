#pragma once

#include <vector>
class Command;

//--------------------------------------------------------------------------------------
// Encapsulates a vector of "Command"s and an index identifying the current command.
// Functionality:
// *Add commands (tryToAddCommand and tryToAddCommand)
// *History jumping (tryToJumpInCommandHistory)
// *Save to byte format (receiveSerializedByteFormat)
// *Load from byte format (tryToLoadFromSerializationByteFormat)
// *Retrieve as std::stringstream (getCommandHistoryAsText)
//--------------------------------------------------------------------------------------
class CommandHistory
{
	friend class CommandHistorySpy;			// Get functions are accessed through "CommandHistorySpy".

private:
	std::vector<Command*> m_commands;		// Stores all commands the "CommandHistory" is responsible for.
	int m_indexOfCurrentCommand;			// -1 means no command is current.
	bool m_historyOverWriteTookPlaceWhenAddingCommands; // Did history overwrite occur when calling "tryToAddCommand" or "tryToAddCommands" the last time?

	// Used internally to set current command.
	void setCurrentCommand(int index);

	// Called from "reset" and the destructor
	void deallocateCommands();

	// Calculates byte size needed to store this command history to binary file.
	int calculateSerializedByteSize();

	// Tracks forward by redoing "nrOfSteps" "Command"s from current.
	void forwardJump(int nrOfSteps);

	// Backtracks by undoing "nrOfSteps" "Command"s from current.
	void backwardJump(int nrOfSteps);

public:
	// Calls "reset".
	CommandHistory(void);

	// Calls "reset".
	~CommandHistory(void);
	
	// Adds a single "Command". Returns true if successfully added, otherwise false.
	bool tryToAddCommand(Command* command, bool execute);

	// Adds a vector of "Command"s. Returns true if successfully added, otherwise false.
	bool tryToAddCommands(std::vector<Command*>*commands, bool execute);

	// Backtracks by undoing all commands from current until index is reached, OR track forward by redoing all commands from current until index is reached. Returns true if jump was successful, otherwise false.
	bool tryToJumpInCommandHistory(int jumpToIndex);

	// "byteSize" is a return value. Refer to .cpp file for format description.
	char* receiveSerializedByteFormat(int& byteSize); 

	// "bytes" is assumed to be in the serialized byte format as returned from "receiveSerializedByteFormat". "byteSize" is assumed to be the size in bytes of the "bytes" parameter. "executeUpAndUntilCurrent" specifies wheter to execute up and untill current (standard: true) or to only load the command history data (false) without executing the commands.
	bool tryToLoadFromSerializationByteFormat(char* bytes, int byteSize, bool executeUpAndUntilCurrent = true);

	// Returns the command history information as text.
	std::stringstream* getCommandHistoryAsText();

	// Jump to beginning, set current command to -1 and delete all commands.
	void reset();
};