#pragma once

#include "EventManager.h"

enum EventType
{
	// Normal events
	EVENT_QUIT_TO_DESKTOP,
	EVENT_WINDOW_RESIZE,
	EVENT_SHOW_MESSAGEBOX,
	EVENT_SET_TOOL,
	EVENT_SET_BACKBUFFER_COLOR,
	EVENT_MOUSE_WHEEL,
	EVENT_MOUSE_PRESS,
	EVENT_MOUSE_MOVE,

	// Commands
	EVENT_STORE_COMMAND,
	EVENT_ADD_COMMAND_TO_COMMAND_HISTORY_GUI,
	EVENT_SET_SELECTED_COMMAND_GUI,
	EVENT_REMOVE_SPECIFIED_COMMANDS_FROM_COMMAND_HISTORY_GUI,

	// Events used to retrieve something
	EVENT_GET_WINDOW_HANDLE,

	// This is needed, don't touch!
	EVENT_LAST 
};


/// Base class
class IEvent
{
private:
	EventType m_type;

public:
	IEvent(EventType p_type)
	{ 
		m_type = p_type;
	}

	EventType type()
	{ 
		return m_type; 
	}
};


/**
EVENTS 
*/

class Event_GetWindowHandle : public IEvent
{
public:
	HWND handle;		

public:
	Event_GetWindowHandle() : IEvent(EVENT_GET_WINDOW_HANDLE)
	{
	}
};

class Event_WindowResize : public IEvent
{
public:
	int width;		
	int height;

public:
	Event_WindowResize(int p_width, int p_height) : IEvent(EVENT_WINDOW_RESIZE)
	{
		width = p_width;
		height = p_height;
	}

	float aspectRatio()
	{
		return (float)width/(float)height;
	}
};

class Event_SetBackBufferColor : public IEvent
{
public:
	float x,y,z;

public:
	Event_SetBackBufferColor(float p_x, float p_y, float p_z) : IEvent(EVENT_SET_BACKBUFFER_COLOR)
	{
		x = p_x;
		y = p_y;
		z = p_z;
	}
};

class Event_ShowMessageBox : public IEvent
{
public:
	std::string message;

public:
	Event_ShowMessageBox(std::string p_message) : IEvent(EVENT_SHOW_MESSAGEBOX)
	{
		message = p_message;
	}
};


class Event_MouseMove : public IEvent
{
public:
	int x;
	int y;
	int dx;
	int dy;

public:
	Event_MouseMove(int x, int y, int dx, int dy) : IEvent(EVENT_MOUSE_MOVE)
	{
		this->x = x;
		this->y = y;

		this->dx = dx;
		this->dy = dy;
	}
};

class Event_MousePress : public IEvent
{
public:
	int x;
	int y;
	int keyEnum;
	bool isPressed;
	
	Event_MousePress(int p_x, int p_y, int p_keyEnum, bool p_isPressed) : IEvent(EVENT_MOUSE_PRESS)
	{
		x = p_x;
		y = p_y;
		keyEnum = p_keyEnum;
		isPressed = p_isPressed;
	}
};

class Event_MouseWheel : public IEvent
{
public:
	int value;

	Event_MouseWheel( int value ) : IEvent(EVENT_MOUSE_WHEEL)
	{
		this->value = value;
	}
};

class Command;
class Event_StoreCommandInCommandHistory : public IEvent
{
public:
	Command* command;
	bool execute; //Call "doRedo" on the command before storing it in the command history. Standard is false. Specify true if the doings of the command is not done when sending this event.

public:
	Event_StoreCommandInCommandHistory(Command* command, bool execute = false) : IEvent(EVENT_STORE_COMMAND)
	{
		this->command = command;
		this->execute = execute;
	}
};

class Event_AddCommandToCommandHistoryGUI : public IEvent
{
public:
	Command* command;

public:
	Event_AddCommandToCommandHistoryGUI(Command* command) : IEvent(EVENT_ADD_COMMAND_TO_COMMAND_HISTORY_GUI)
	{
		this->command = command;
	}
};

class Event_RemoveCommandsFromCommandHistoryGUI : public IEvent
{
public:
	int startIndex; //Index of first command to be removed
	int nrOfCommands; //Counting from "startIndex". Standard is "1", meaning that one command will be removed, the command at "startIndex".

public:
	Event_RemoveCommandsFromCommandHistoryGUI(int startindex, int nrOfCommands = 1) : IEvent(EVENT_REMOVE_SPECIFIED_COMMANDS_FROM_COMMAND_HISTORY_GUI)
	{
		this->startIndex = startIndex;
		this->nrOfCommands = nrOfCommands;
	}
};

class Event_SetSelectedCommandGUI : public IEvent
{
public:
	int indexOfCommand;

public:
	Event_SetSelectedCommandGUI(int indexOfCommand) : IEvent(EVENT_SET_SELECTED_COMMAND_GUI)
	{
		this->indexOfCommand = indexOfCommand;
	}
};
