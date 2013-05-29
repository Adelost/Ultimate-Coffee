#pragma once

class Command;
class CommandHistory;

//friend class of "CommandHistory"
class CommandHistorySpy
{
private:
	CommandHistory* m_commandHistory;

public:
	CommandHistorySpy(CommandHistory* commandHistory);
	~CommandHistorySpy(void);

	std::vector<Command*>* getCommands();
	int getIndexOfCurrentCommand();
	int getNrOfCommands();
	bool getHistoryOverWriteTookPlaceWhenAddingCommands();
};
