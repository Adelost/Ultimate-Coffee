#pragma once

typedef struct HWND__* HWND;
struct DirectX::SimpleMath::Color;

class Settings
{
public:
	float deltaTime;
	HWND windowHandle;
	bool leftMousePressed;
	Color* backBufferColor;

public:
	Settings();
	~Settings();
};
