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
	Settings();

	~Settings();
};
