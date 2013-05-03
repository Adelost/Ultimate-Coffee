#pragma once

#include "Math.h"

typedef struct HWND__* HWND;
class Camera;
class Entity;

class Settings
{
public:
	float deltaTime;
	HWND windowHandle;
	bool leftMousePressed;
	Color backBufferColor;
	int cameraEntityId;
	int selectedTool;
	int selectedEntityId;
	int camera_entityId;

public:
	Settings();
	~Settings();
};
