#include "System_Render.h"

#include "Core/EventManager.h"

#include "DXRenderer.h"

System::Render::Render()
{
	setupDirectX();
}

void System::Render::setupDirectX()
{
	HWND windowHandle;
	{
		Event_GetWindowHandle e;
		SEND_EVENT(&e);
		windowHandle = e.handle;
	}

	renderer = new DXRenderer();
	renderer->init(windowHandle);
}

void System::Render::process()
{

}

System::Render::~Render()
{
	delete renderer;
}

void System::Render::update()
{
	renderer->renderFrame();
}
