#include "Settings.h"

Settings::Settings()
{
	deltaTime = 0.0f;
	windowHandle = nullptr;
	backBufferColor = Color(0.4f, 0.6f, 0.9f);
}

Settings::~Settings()
{
	delete windowHandle;
}
