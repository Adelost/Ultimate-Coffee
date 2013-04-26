#pragma once

#include "Util.h"

class Settings
{
public:
	float deltaTime;
	HWND windowHandle;
	Color backBufferColor;

public:
	Settings();
	~Settings();
};
