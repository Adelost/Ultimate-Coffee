#pragma once

#include "Math.h"
typedef struct HWND__* HWND;
class Camera;

class Settings
{
public:
	float deltaTime;
	HWND windowHandle;
	bool leftMousePressed;
	Color backBufferColor;
	Camera* camera;

public:
	Settings();
	~Settings();
};
