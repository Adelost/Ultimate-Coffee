#pragma once

#include "Math.h"
#include "EntityPointer.h"
#include "Enums.h"

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

	bool key_shift;
	bool key_ctrl;
	bool key_alt;
	bool key_space;
};

class ChoosenEntity
{
public:
	Color color;
	Enum::Mesh mesh;

public:
	ChoosenEntity()
	{
		color = Color(0.4f, 0.6f, 0.9f);
		mesh = Enum::Mesh_Box;
	}
};

class Settings
{
private:
	int m_selectedTool;
	int m_skyboxIndex;
	bool m_runSimulation;

public:
	ChoosenEntity choosenEntity;
	ButtonState button;
	Color backBufferColor;
	EntityPointer entity_selection;
	EntityPointer entity_camera;
	HWND windowHandle;
	Int2 windowSize;
	float deltaTime;

	/**
	Always shows elapsed time, even 
	if game is paused.
	*/
	float trueDeltaTime;

	int DXViewPortTopLeftX;
	int DXViewPortTopLeftY;
	int DXViewPortMinDepth;
	int DXViewPortMaxDepth;
	Int2 lastMousePosition;
	
	Enum::ColorScheme m_ColorScheme_3DManipulatorWidgets;

public:
	Settings();
	~Settings();

public:
	void setSelectedTool(int toolType);
	int selectedTool(){return m_selectedTool;}
	void setSkyboxIndex(int index);
	int skyboxIndex();
	void setRunSimulation(bool state){m_runSimulation = state;}
	bool runSimulation(){return m_runSimulation;}
};