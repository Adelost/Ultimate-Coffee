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
	EVENT_PREVIEW_ITEMS,
	EVENT_ENTITY_SELECTION,
	EVENT_START_MULTISELECT,
	EVENT_COFFEE,
	EVENT_SELECTED_ENTITIES_HAVE_BEEN_TRANSFORMED,

	EVENT_PLAY_SOUND_DING,

	// Commands
	EVENT_ADD_TO_COMMAND_HISTORY,
	EVENT_ADD_TO_COMMAND_HISTORY_GUI,
	EVENT_REMOVE_ALL_COMMANDS_FROM_CURRENT_ROW_IN_COMMAND_HISTORY_GUI,
	EVENT_JUMP_TO_COMMAND_HISTORY_INDEX,
	EVENT_SET_SELECTED_COMMAND_GUI,
	EVENT_GET_COMMAND_HISTORY_INFO,
	EVENT_ADD_ROOT_COMMAND_TO_COMMAND_HISTORY_GUI,
	EVENT_INCREMENT_OR_DECREMENT_CURRENT_ROW_IN_COMMAND_HISTORY_GUI,
	EVENT_GET_COMMAND_HISTORY_GUI_FILTER,

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
		SceneCursor_Pointer = 21,
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
// Adds a single command or a list of commands to the command history
class Event_AddToCommandHistory : public Event
{
public:
	std::vector<Command*>* commands; // List of commands that should be added to the command history.
	Command* command; // Special case needed for backward compatibility with single-command additions. If this is NULL, "commands" is used instead.
	bool execute; // Call "doRedo" on the commands before storing them in the command history. Standard is false. Specify true if the doings of the command is not done when sending the event.
	bool displayAsSingleCommandHistoryEntry; // Refer to "Event_AddToCommandHistoryGUI". Standard is true.

public:
	Event_AddToCommandHistory(std::vector<Command*>* commands, bool execute = false, bool displayAsSingleCommandHistoryEntry = true) : Event(EVENT_ADD_TO_COMMAND_HISTORY)
	{
		this->commands = commands;
		this->command = NULL;
		this->execute = execute;
		this->displayAsSingleCommandHistoryEntry = displayAsSingleCommandHistoryEntry;
	}

	Event_AddToCommandHistory(Command* command, bool execute = false) : Event(EVENT_ADD_TO_COMMAND_HISTORY)
	{
		this->commands = NULL;
		this->command = command;
		this->execute = execute;
		this->displayAsSingleCommandHistoryEntry = true;
	}
};

class Event_SelectedEntitiesHaveBeenTransformed : public Event
{
public:
	bool m_transX, m_transY, m_transZ, m_scaleX, m_scaleY, m_scaleZ, m_rotX, m_rotY, m_rotZ;

public:
	Event_SelectedEntitiesHaveBeenTransformed() : Event(EVENT_SELECTED_ENTITIES_HAVE_BEEN_TRANSFORMED)
	{
		m_transX = false;
		m_transY = false;
		m_transZ = false;
		m_scaleX = false;
		m_scaleY = false;
		m_scaleZ = false;
		m_rotX = false;
		m_rotY = false;
		m_rotZ = false;
	}
};

class Event_AddToCommandHistoryGUI : public Event
{
public:
	std::vector<Command*>* commands;
	bool displayAsSingleCommandHistoryEntry; // Example display of true: "Entity creation (43)". Example display of false: "Entity creation".
	int indexToBundledWithCommandHistoryGUIListEntry; // Standard is -2. If the standard value is not overridden the value will be set to the current command history index as given by "CommandHistory".

public:
	Event_AddToCommandHistoryGUI(std::vector<Command*>* commands, bool displayAsSingleCommandHistoryEntry, int indexToBundledWithCommandHistoryGUIListEntry = -2) : Event(EVENT_ADD_TO_COMMAND_HISTORY_GUI)
	{
		this->commands = commands;
		this->displayAsSingleCommandHistoryEntry = displayAsSingleCommandHistoryEntry;
		this->indexToBundledWithCommandHistoryGUIListEntry = indexToBundledWithCommandHistoryGUIListEntry;
	}
};

class Event_RemoveAllCommandsAfterCurrentRowFromCommandHistoryGUI : public Event
{
public:
	bool removeAllCommands; // Standard is false

public:
	Event_RemoveAllCommandsAfterCurrentRowFromCommandHistoryGUI(bool removeAllCommands = false) : Event(EVENT_REMOVE_ALL_COMMANDS_FROM_CURRENT_ROW_IN_COMMAND_HISTORY_GUI)
	{
		this->removeAllCommands = removeAllCommands;
	}
};

// Backtracks by undoing until a command index is reached, or track forward by redoing until command index is reached
class Event_JumpToCommandHistoryIndex : public Event
{
public:
	int commandHistoryIndex;

public:
	Event_JumpToCommandHistoryIndex(int commandHistoryIndex) : Event(EVENT_JUMP_TO_COMMAND_HISTORY_INDEX)
	{
		this->commandHistoryIndex = commandHistoryIndex;
	}
};

class Event_SetSelectedCommandGUI : public Event
{
public:
	int commandHistoryIndex;

public:
	Event_SetSelectedCommandGUI(int commandHistoryIndex) : Event(EVENT_SET_SELECTED_COMMAND_GUI)
	{
		this->commandHistoryIndex = commandHistoryIndex;
	}
};

class Event_GetCommandHistoryInfo : public Event
{
public:
	int indexOfCurrentCommand;
	int nrOfCommands;

public:
	Event_GetCommandHistoryInfo() : Event(EVENT_GET_COMMAND_HISTORY_INFO)
	{
		indexOfCurrentCommand = -2;
		nrOfCommands = -1;
	}
};

class Event_GetCommandHistoryGUIFilter : public Event
{
public:
	std::vector<bool>* GUIFilter; // Return value

public:
	Event_GetCommandHistoryGUIFilter() : Event(EVENT_GET_COMMAND_HISTORY_GUI_FILTER)
	{
	}
};

class Event_IncrementOrDecrementCurrentRowInCommandHistoryGUI : public Event
{
public:
	bool if_true_increment_if_false_decrement;

public:
	Event_IncrementOrDecrementCurrentRowInCommandHistoryGUI(bool if_true_increment_if_false_decrement) : Event(EVENT_INCREMENT_OR_DECREMENT_CURRENT_ROW_IN_COMMAND_HISTORY_GUI)
	{
		this->if_true_increment_if_false_decrement = if_true_increment_if_false_decrement;
	}
};