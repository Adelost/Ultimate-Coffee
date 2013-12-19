#include "stdafx.h"
#include "System_Render.h"
#include "DXRenderer.h"



System::Render::Render()
{
	Data::Render::initClass();
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

System::Render::~Render()
{
	delete renderer;
}

void System::Render::update()
{
	// Render
	renderer->renderFrame();
}

