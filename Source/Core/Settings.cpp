#include "stdafx.h"
#include "Settings.h"
#include "Camera.h"

Settings::Settings()
{
	leftMousePressed = false;
	deltaTime = 0.0f;
	windowHandle = nullptr;
	backBufferColor = Color(0.4f, 0.6f, 0.9f);
	camera = new Camera();
}

Settings::~Settings()
{
	delete windowHandle;
	delete camera;
}
