#include "stdafx.h"
#include "Settings.h"
#include "Enums.h"
#include "Events.h"

Settings::Settings()
{
	m_selectedTool = Enum::Tool_None;
	deltaTime = 0.0f;
	windowHandle = nullptr;
	backBufferColor = Color(0.4f, 0.6f, 0.9f);
	showSkybox = true;
}

Settings::~Settings()
{
	delete windowHandle;
}

void Settings::setSelectedTool( int toolType )
{
	// Only switch if we are changing tools
	if(m_selectedTool != toolType)
	{
		m_selectedTool = toolType;

		// Inform about switch
		DEBUGPRINT("SELECTED TOOL: " + Converter::IntToStr(m_selectedTool));
		SEND_EVENT(&Event(EVENT_SET_TOOL));
	}		
}

int Settings::selectedTool()
{
	return m_selectedTool;
}

ButtonState::ButtonState()
{
	ZeroMemory(this, sizeof(ButtonState));
}
