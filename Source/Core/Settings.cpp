#include "stdafx.h"
#include "Settings.h"
#include "Enums.h"

Settings::Settings()
{
	selectedTool = Enum::Tool_None;
	leftMousePressed = false;
	deltaTime = 0.0f;
	windowHandle = nullptr;
	backBufferColor = Color(0.4f, 0.6f, 0.9f);
	selectedEntityId = -1;
	camera_entityId = -1;
}

Settings::~Settings()
{
	delete windowHandle;
}
