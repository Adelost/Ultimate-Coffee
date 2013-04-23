#pragma once

#include "windows.h"

class Settings
{
public:
	float deltaTime;
	HWND windowHandle;

public:
	Settings()
	{
		deltaTime = 0.0f;
		windowHandle = nullptr;
	}

	~Settings()
	{
		delete windowHandle;
	}
};
