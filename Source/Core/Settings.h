#pragma once

#include "Math.h"
#include "EntityPointer.h"

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
	ButtonState button;
	Color backBufferColor;
	EntityPointer entity_selection;
	EntityPointer entity_camera;
	HWND windowHandle;
	Int2 windowSize;
	float deltaTime;
	int selectedTool;

	int DXViewPortTopLeftX;
	int DXViewPortTopLeftY;
	int DXViewPortMinDepth;
	int DXViewPortMaxDepth;

public:
	Settings();
	~Settings();
};
