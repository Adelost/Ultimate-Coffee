#include "stdafx.h"
#include "Command_ChangeBackBufferColor.h"
#include "Events.h"

Command_ChangeBackBufferColor::Command_ChangeBackBufferColor()
{
	m_commandData.doX=0.0f;
	m_commandData.doY=0.0f;
	m_commandData.doZ=0.0f;
	m_commandData.undoX=0.0f;
	m_commandData.undoY=0.0f;
	m_commandData.undoZ=0.0f;

	setType(Enum::CommandType::CHANGEBACKBUFFERCOLOR);
}

Command_ChangeBackBufferColor::~Command_ChangeBackBufferColor(void)
{
}

void Command_ChangeBackBufferColor::doRedo()
{
	SEND_EVENT(&Event_SetBackBufferColor(m_commandData.doX, m_commandData.doY, m_commandData.doZ));
}

void Command_ChangeBackBufferColor::undo()
{
	SEND_EVENT(&Event_SetBackBufferColor(m_commandData.undoX, m_commandData.undoY, m_commandData.undoZ));
}