#include "Command_ChangeBackBufferColor.h"
#include "EventManager.h"
#include "Events.h"

Command_ChangeBackBufferColor::Command_ChangeBackBufferColor()
{
	commandData_.doX=0.0f;
	commandData_.doY=0.0f;
	commandData_.doZ=0.0f;
	commandData_.undoX=0.0f;
	commandData_.undoY=0.0f;
	commandData_.undoZ=0.0f;

	setType(Enum::CommandType::CHANGEBACKBUFFERCOLOR);
}

Command_ChangeBackBufferColor::~Command_ChangeBackBufferColor(void)
{
}

void Command_ChangeBackBufferColor::doRedo()
{
	SEND_EVENT(&Event_SetBackBufferColor(commandData_.doX, commandData_.doY, commandData_.doZ));
}

void Command_ChangeBackBufferColor::undo()
{
	SEND_EVENT(&Event_SetBackBufferColor(commandData_.undoX, commandData_.undoY, commandData_.undoZ));
}