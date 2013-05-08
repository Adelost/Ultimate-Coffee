#include "stdafx.h"
#include "Settings.h"
#include "Enums.h"

Settings::Settings()
{
	selectedTool = Enum::Tool_None;
	deltaTime = 0.0f;
	windowHandle = nullptr;
	backBufferColor = Color(0.4f, 0.6f, 0.9f);
}

Settings::~Settings()
{
	delete windowHandle;
}

ButtonState::ButtonState()
{
	ZeroMemory(this, sizeof(ButtonState));
}
