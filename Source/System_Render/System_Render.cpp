#include "stdafx.h"
#include "System_Render.h"
#include "DXRenderer.h"

#include <Core/EventManager.h>
#include <Core/Events.h>
#include <Core/Camera.h>
#include <Core/World.h>
#include <Core/Settings.h>

#include <Core/Enums.h>

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
	DataMapper<Data::Transform> m_mapper_position;
	while(m_mapper_position.hasNext())
	{
		Data::Transform* transform = m_mapper_position.next();
	}
}

System::Render::~Render()
{
	delete renderer;
}

void System::Render::update()
{
	renderer->renderFrame();
}

