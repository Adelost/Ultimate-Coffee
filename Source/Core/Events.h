#pragma once

#include "EventManager.h"

enum EventType
{
	// Normal events
	EVENT_QUIT_TO_DESKTOP,
	EVENT_WINDOW_RESIZE,
	EVENT_SHOW_MESSAGEBOX,
	EVENT_SET_BACKBUFFER_COLOR,

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