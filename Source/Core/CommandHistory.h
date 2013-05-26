#pragma once

#include <vector>
class Command;

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
	friend class CommandHistorySpy;
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
	bool tryToAddCommand(Command* command, bool execute);
	
	// Backtracks by undoing all commands from current until index is reached, OR track forward by redoing all commands from current until index is reached. Returns true if jump was successful, otherwise false.
	bool tryToJumpInCommandHistory(int jumpToIndex);

	// "byteSize" is a return value. Refer to .cpp file for format description.
	char* receiveSerializedByteFormat(int& byteSize); 

	// "bytes" is assumed to be in the serialized byte format as returned from "receiveSerializedByteFormat". "byteSize" is assumed to be the size in bytes of the "bytes" parameter.
	bool tryToLoadFromSerializationByteFormat(char* bytes, int byteSize);

	// Returns the command history information as text
	std::stringstream* getCommandHistoryAsText();

	void reset();
};