#pragma once

#include "windows.h"

class Settings
{
public:
	float deltaTime;
	HWND windowHandle;
	float backBufferColorX;
	float backBufferColorY;
	float backBufferColorZ;

public:
	Settings()
	{
		backBufferColorX = 0.0f;
		backBufferColorY = 0.0f;
		backBufferColorZ = 0.0f;
		deltaTime = 0.0f;
		windowHandle = nullptr;
	}

	~Settings()
	{
		delete windowHandle;
	}
};
