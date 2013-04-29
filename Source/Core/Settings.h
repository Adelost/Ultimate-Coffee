#pragma once

#include "Math.h"
typedef struct HWND__* HWND;

class Settings
{
public:
	float deltaTime;
	HWND windowHandle;
	bool leftMousePressed;
	Color backBufferColor;

public:
	Settings();
	~Settings();
};
