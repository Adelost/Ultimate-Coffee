#include "Settings.h"

Settings::Settings()
{
	backBufferColorX = 0.4f;
	backBufferColorY = 0.6f;
	backBufferColorZ = 0.9f;
	deltaTime = 0.0f;
	windowHandle = nullptr;
}

Settings::~Settings()
{
	delete windowHandle;
}
