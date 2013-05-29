#include "stdafx.h"
#include "CommandHistorySpy.h"
#include "CommandHistory.h"

CommandHistorySpy::CommandHistorySpy(CommandHistory* commandHistory)
{
	m_commandHistory = commandHistory;
}

CommandHistorySpy::~CommandHistorySpy(void)
{
}

std::vector<Command*>* CommandHistorySpy::getCommands()
{
	return &m_commandHistory->m_commands;
}

int CommandHistorySpy::getIndexOfCurrentCommand()
{
	return m_commandHistory->m_indexOfCurrentCommand;
}

int CommandHistorySpy::getNrOfCommands()
{
	return m_commandHistory->m_commands.size();
}

bool CommandHistorySpy::getHistoryOverWriteTookPlaceWhenAddingCommands()
{
	return m_commandHistory->m_historyOverWriteTookPlaceWhenAddingCommands;
}