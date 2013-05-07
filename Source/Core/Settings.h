#pragma once

#include "Math.h"

typedef struct HWND__* HWND;
class Camera;
class Entity;

class ButtonState
{
public:
	ButtonState();
public:
	bool mouse_left;
	bool mouse_right;
	bool mouse_middle;
	bool key_up;
	bool key_down;
	bool key_left;
	bool key_right;
};

class Settings
{
public:
	float deltaTime;
	HWND windowHandle;
	Color backBufferColor;
	int selectedTool;
	int selectedEntityId;
	int camera_entityId;
	ButtonState button;
	Int2 windowSize;

	int DXViewPortTopLeftX;
	int DXViewPortTopLeftY;
	int DXViewPortMinDepth;
	int DXViewPortMaxDepth;

public:
	Settings();
	~Settings();
};
