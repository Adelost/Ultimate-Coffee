#pragma once

#include "EventManager.h"
typedef struct HWND__* HWND;

enum EventType
{
	// Normal events
	EVENT_QUIT_TO_DESKTOP,
	EVENT_NEW_PROJECT,
	EVENT_WINDOW_RESIZE,
	EVENT_SHOW_MESSAGEBOX,
	EVENT_SET_TOOL,
	EVENT_SKYBOX_CHANGED,
	EVENT_SET_BACKBUFFER_COLOR,
	EVENT_TRANSLATE_SCENE_ENTITY,
	EVENT_ROTATE_SCENE_ENTITY,
	EVENT_SCALE_SCENE_ENTITY,
	EVENT_MOUSE_WHEEL,
	EVENT_MOUSE_PRESS,
	EVENT_MOUSE_MOVE,
	EVENT_SET_CURSOR_POSITION,
	EVENT_SET_CURSOR,
	EVENT_REFRESH_SPLITTER,
	EVENT_ENTITY_SELECTION,
	EVENT_START_MULTISELECT,
	EVENT_COFFEE,

	// Commands
	EVENT_STORE_COMMAND,
	EVENT_STORE_COMMANDS_AS_SINGLE_COMMAND_HISTORY_GUI_ENTRY,
	EVENT_ADD_COMMAND_TO_COMMAND_HISTORY_GUI,
	EVENT_SET_SELECTED_COMMAND_GUI,
	EVENT_REMOVE_SPECIFIED_COMMANDS_FROM_COMMAND_HISTORY_GUI,
	EVENT_TRACK_TO_COMMAND_HISTORY_INDEX,
	EVENT_GET_COMMANDER_INFO,
	EVENT_GET_NEXT_VISIBLE_COMMAND_ROW,

	// Events used to retrieve something
	EVENT_GET_WINDOW_HANDLE,

	// This is needed, don't touch!
	EVENT_LAST 
};


/// Base class
class Event
{
private:
	EventType m_type;

public:
	Event(EventType p_type)
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

class Event_GetWindowHandle : public Event
{
public:
	HWND handle;		

public:
	Event_GetWindowHandle() : Event(EVENT_GET_WINDOW_HANDLE)
	{
	}
};

class Event_WindowResize : public Event
{
public:
	int width;		
	int height;

public:
	Event_WindowResize(int p_width, int p_height) : Event(EVENT_WINDOW_RESIZE)
	{
		width = p_width;
		height = p_height;
	}

	float aspectRatio()
	{
		return (float)width/(float)height;
	}
};

class Event_SetBackBufferColor : public Event
{
public:
	float x,y,z;

public:
	Event_SetBackBufferColor(float p_x, float p_y, float p_z) : Event(EVENT_SET_BACKBUFFER_COLOR)
	{
		x = p_x;
		y = p_y;
		z = p_z;
	}
};

class Event_TranslateSceneEntity : public Event
{
public:
	int m_idOfTranslatableSceneEntity;
	float m_transX, m_transY, m_transZ;

public:
	Event_TranslateSceneEntity(int p_idOfTranslatableSceneEntity, float p_transX, float p_transY, float p_transZ) : Event(EVENT_TRANSLATE_SCENE_ENTITY)
	{
		m_idOfTranslatableSceneEntity = p_idOfTranslatableSceneEntity;
		m_transX = p_transX;
		m_transY = p_transY;
		m_transZ = p_transZ;
	}
};

class Event_RotateSceneEntity : public Event
{
public:
	int m_idOfRotatableSceneEntity;
	float m_quatX, m_quatY, m_quatZ, m_quatW;

public:
	Event_RotateSceneEntity(int p_idOfRotatableSceneEntity, float p_quatX, float p_quatY, float p_quatZ, float p_quatW) : Event(EVENT_ROTATE_SCENE_ENTITY)
	{
		m_idOfRotatableSceneEntity = p_idOfRotatableSceneEntity;
		m_quatX = p_quatX;
		m_quatY = p_quatY;
		m_quatZ = p_quatZ;
		m_quatW = p_quatW;
	}
};

class Event_ScaleSceneEntity : public Event
{
public:
	int m_idOfScalableSceneEntity;
	float m_scaleX, m_scaleY, m_scaleZ;

public:
	Event_ScaleSceneEntity(int p_idOfScalableSceneEntity, float p_scaleX, float p_scaleY, float p_scaleZ) : Event(EVENT_SCALE_SCENE_ENTITY)
	{
		m_idOfScalableSceneEntity = p_idOfScalableSceneEntity;
		m_scaleX = p_scaleX;
		m_scaleY = p_scaleY;
		m_scaleZ = p_scaleZ;
	}
};

class Event_ShowMessageBox : public Event
{
public:
	std::string message;

public:
	Event_ShowMessageBox(std::string p_message) : Event(EVENT_SHOW_MESSAGEBOX)
	{
		message = p_message;
	}
};

class Event_SetCursor : public Event
{
public:
	enum CursorShape
	{
		HiddenCursor = -2,
		NormalCursor = -1,
		CrossCursor = 2,
		SizeAllCursor = 9,
		OpenHandCursor = 17,
		ClosedHandCursor = 18,
		SceneCursor = 20,
	};

public:
	CursorShape cursorShape;

public:
	Event_SetCursor(CursorShape p_cursor) : Event(EVENT_SET_CURSOR)
	{
		cursorShape = p_cursor;
	}
};

class Event_MouseMove : public Event
{
public:
	int x;
	int y;
	int dx;
	int dy;

public:
	Event_MouseMove(int x, int y, int dx, int dy) : Event(EVENT_MOUSE_MOVE)
	{
		this->x = x;
		this->y = y;

		this->dx = dx;
		this->dy = dy;
	}
};

class Event_MousePress : public Event
{
public:
	int x;
	int y;
	int keyEnum;
	bool isPressed;
	
	Event_MousePress(int p_x, int p_y, int p_keyEnum, bool p_isPressed) : Event(EVENT_MOUSE_PRESS)
	{
		x = p_x;
		y = p_y;
		keyEnum = p_keyEnum;
		isPressed = p_isPressed;
	}
};

class Event_MouseWheel : public Event
{
public:
	int value;

	Event_MouseWheel( int value ) : Event(EVENT_MOUSE_WHEEL)
	{
		this->value = value;
	}
};

class Event_SetCursorPosition : public Event
{
public:
	Int2 position;

public:
	Event_SetCursorPosition( Int2 position ) : Event(EVENT_SET_CURSOR_POSITION)
	{
		this->position = position;
	}
};

class Command;
class Event_StoreCommandInCommandHistory : public Event
{
public:
	Command* command;
	bool execute; // Call "doRedo" on the command before storing it in the command history. Standard is false. Specify true if the doings of the command is not done when sending this event.
	bool setAsCurrentInGUI; // Performance critical when sending lots of "Event_StoreCommandInCommandHistory" events in a row. Set to false if this event is not the last "Event_StoreCommandInCommandHistory" sent in a row. Standard is true, which is fitting for single commands.

public:
	Event_StoreCommandInCommandHistory(Command* command, bool execute = false, bool setAsCurrentInGUI = true) : Event(EVENT_STORE_COMMAND)
	{
		this->command = command;
		this->execute = execute;
		this->setAsCurrentInGUI = setAsCurrentInGUI;
	}
};

class Event_StoreCommandsAsSingleEntryInCommandHistoryGUI : public Event
{
public:
	std::vector<Command*>* commands;
	bool execute; // Whether or not to execute the commands when adding them to the command history. Standard is false.
	
public:
	Event_StoreCommandsAsSingleEntryInCommandHistoryGUI(std::vector<Command*>* commands, bool execute = false) : Event(EVENT_STORE_COMMANDS_AS_SINGLE_COMMAND_HISTORY_GUI_ENTRY)
	{
		this->commands = commands;
		this->execute = execute;
	}
};

class Event_AddCommandToCommandHistoryGUI : public Event
{
public:
	Command* command;
	bool hidden;
	int mergeNumber;

public:
	Event_AddCommandToCommandHistoryGUI(Command* command, bool hidden, int mergeNumber = 0) : Event(EVENT_ADD_COMMAND_TO_COMMAND_HISTORY_GUI)
	{
		this->command = command;
		this->hidden = hidden;
		this->mergeNumber = mergeNumber;
	}
};

class Event_RemoveCommandsFromCommandHistoryGUI : public Event
{
public:
	int startIndex; // Index of first command to be removed
	int nrOfCommands; // Counting from "startIndex". Standard is "1", meaning that one command will be removed, the command at "startIndex".

public:
	Event_RemoveCommandsFromCommandHistoryGUI(int startIndex, int nrOfCommands = 1) : Event(EVENT_REMOVE_SPECIFIED_COMMANDS_FROM_COMMAND_HISTORY_GUI)
	{
		this->startIndex = startIndex;
		this->nrOfCommands = nrOfCommands;
	}
};

// Backtracks by undoing until a command index is reached, or track forward by redoing until command index is reached
class Event_TrackToCommandHistoryIndex : public Event
{
public:
	int indexOfCommand;

public:
	Event_TrackToCommandHistoryIndex(int indexOfCommand) : Event(EVENT_TRACK_TO_COMMAND_HISTORY_INDEX)
	{
		this->indexOfCommand = indexOfCommand;
	}
};

class Event_SetSelectedCommandGUI : public Event
{
public:
	int indexOfCommand;

public:
	Event_SetSelectedCommandGUI(int indexOfCommand) : Event(EVENT_SET_SELECTED_COMMAND_GUI)
	{
		this->indexOfCommand = indexOfCommand;
	}
};

class Event_GetCommanderInfo : public Event
{
public:
	int indexOfCurrentCommand;
	int nrOfCommands;

public:
	Event_GetCommanderInfo() : Event(EVENT_GET_COMMANDER_INFO)
	{
	}
};

class Event_GetNextOrPreviousVisibleCommandRowInCommandHistory : public Event
{
public:
	int row; // Return value
	bool next; // Next if true, previous if false.
	int currentCommandHistoryIndex; // Needed for special case when jumping in and out of history (refer to Event_GetNextOrPreviousVisibleCommandRowInCommandHistory)

public:
	Event_GetNextOrPreviousVisibleCommandRowInCommandHistory(bool next, int currentCommandHistoryIndex) : Event(EVENT_GET_NEXT_VISIBLE_COMMAND_ROW)
	{
		this->next = next;
		this->currentCommandHistoryIndex = currentCommandHistoryIndex;
	}
};
