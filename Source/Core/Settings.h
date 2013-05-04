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
	int selectedTool;
	int selectedEntityId;
	int camera_entityId;

public:
	Settings();
	~Settings();
};
