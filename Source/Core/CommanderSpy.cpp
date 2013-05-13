#include "stdafx.h"
#include "CommanderSpy.h"
#include "Commander.h"

CommanderSpy::CommanderSpy(Commander* commander)
{
	m_commander = commander;
}

CommanderSpy::~CommanderSpy(void)
{
}

std::vector<Command*>* CommanderSpy::getCommands()
{
	return &m_commander->m_commandHistory->m_commands;
}