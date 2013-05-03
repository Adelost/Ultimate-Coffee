#include "stdafx.h"
#include "System_Render.h"
#include "DXRenderer.h"

#include <Core/EventManager.h>
#include <Core/Events.h>
#include <Core/Camera.h>
#include <Core/World.h>
#include <Core/Settings.h>

System::Render::Render()
{
	setupDirectX();

	currentlyChosenTransformTool = NULL;
	theTranslationTool = new Tool_Translation();
	// HACK: Will fix later.
	currentlyChosenTransformTool = theTranslationTool;
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

	Camera& mCam = *(SETTINGS()->camera);

	// Render/Update tools
	if(currentlyChosenTransformTool)
	{
		if(currentlyChosenTransformTool->getActiveObject() != -1)
		{
			XMFLOAT4X4 toolWorld = currentlyChosenTransformTool->getWorld_logical(); // Use the "logical world" if we don't want it to retain its size even whilst translating an object (could be distracting by giving a "mixed message" re: the object's actual location?)
			XMVECTOR origin = XMLoadFloat4(&XMFLOAT4(toolWorld._41, toolWorld._42, toolWorld._43, 1)); //XMLoadFloat4(&test_toolOrigo);
			float dist = XMVector3Length(XMVectorSubtract(mCam.GetPositionXM(), origin)).m128_f32[0];
			float distanceAdjustedScale = dist / 6;
			currentlyChosenTransformTool->setScale(distanceAdjustedScale);

			if(currentlyChosenTransformTool == theTranslationTool)
				theTranslationTool->updateViewPlaneTranslationControlWorld(mCam.GetLook(), mCam.GetUp());
		}
	}
}

void System::Render::onEvent( IEvent* p_event )
{
	EventType type = p_event->type();
	switch (type) 
	{
	case EVENT_MOUSE_MOVE:
		{
			Event_MouseMove* e = static_cast<Event_MouseMove*>(p_event);
			e->dx;
		}
		break;
	case EVENT_MOUSE_PRESS:
		{
			Event_MousePress* e = static_cast<Event_MousePress*>(p_event);
			e->isPressed;
		}
	default:
		break;
	}
}
