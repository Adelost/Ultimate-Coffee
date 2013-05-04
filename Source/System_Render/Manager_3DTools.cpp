#include "stdafx.h"
#include "Manager_3DTools.h"

#include <Core/Data.h>
#include <Core/DataMapper.h>
#include <Core/Events.h>
#include <Core/World.h>
#include <Core/Enums.h>

#include "ITool_Transformation.h"
#include "Tool_Translation.h"


Manager_3DTools::Manager_3DTools( ID3D11Device* p_device, ID3D11DeviceContext* p_deviceContext, ID3D11DepthStencilView* p_depthStencilView )
{
	m_deviceContext = p_deviceContext;
	m_depthStencilView = p_depthStencilView;

	SUBSCRIBE_TO_EVENT(this, EVENT_MOUSE_PRESS);
	SUBSCRIBE_TO_EVENT(this, EVENT_MOUSE_RELEASE);
	SUBSCRIBE_TO_EVENT(this, EVENT_MOUSE_MOVE);

	// Initialize the transformation tools...
	currentlyChosenTransformTool = NULL;
	m_theTranslationTool = new Tool_Translation();

	// HACK: Hard-coded the chosen transform tool here. To be chosen via toolbar and keyboard shortcuts.
	currentlyChosenTransformTool = m_theTranslationTool;

	m_theTranslationTool->init(p_device, p_deviceContext);

	// TEMP:
	(SETTINGS()->camera)->SetPosition(-15.0f, 0.0f, 0.0f);
	(SETTINGS()->camera)->SetLens(0.25f * Math::Pi, 800.0f / 600.0f, 1.0f, 1000.0f);
	(SETTINGS()->camera)->LookAt(SETTINGS()->camera->GetPosition(), XMFLOAT3(0.0f, 0.0f, 0.0f), SETTINGS()->camera->GetUp());
	(SETTINGS()->camera)->UpdateViewMatrix();
}

void Manager_3DTools::update()
{
	// TEMP:
	if(currentlyChosenTransformTool)
		currentlyChosenTransformTool->setActiveObject(SETTINGS()->selectedEnityId);

	if(currentlyChosenTransformTool && currentlyChosenTransformTool->getActiveObject() != -1)
	{
		Camera theCamera = *SETTINGS()->camera;

		XMFLOAT4X4 toolWorld = currentlyChosenTransformTool->getWorld_logical(); // Use the "logical world" if we don't want it to retain its size even whilst translating an object (could be distracting by giving a "mixed message" re: the object's actual location?)
		XMVECTOR origin = XMLoadFloat4(&XMFLOAT4(toolWorld._41, toolWorld._42, toolWorld._43, 1)); //XMLoadFloat4(&test_toolOrigo);
		float dist = XMVector3Length(XMVectorSubtract(theCamera.GetPositionXM(), origin)).m128_f32[0];
		float distanceAdjustedScale = dist / 6;
		currentlyChosenTransformTool->setScale(distanceAdjustedScale);

		if(currentlyChosenTransformTool == m_theTranslationTool)
			m_theTranslationTool->updateViewPlaneTranslationControlWorld(theCamera.GetLook(), theCamera.GetUp());
	}
}

void Manager_3DTools::draw(ID3D11DepthStencilView* p_depthStencilView)
{
	if(currentlyChosenTransformTool && currentlyChosenTransformTool->getActiveObject() != -1)
	{
		Camera theCamera = *SETTINGS()->camera;

		//m_deviceContext->ClearDepthStencilView(p_depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

		currentlyChosenTransformTool->draw(theCamera, p_depthStencilView);
	}
}

void Manager_3DTools::onEvent( IEvent* p_event )
{
	EventType type = p_event->type();

	switch (type) 
	{
	case EVENT_MOUSE_PRESS:
		{
			Event_MousePress* e = static_cast<Event_MousePress*>(p_event);
			e->isPressed;

			if(e->keyEnum == Enum::QtKeyPress_MouseLeft)
			{
				Vector2 clickedScreenCoords((int)e->x, (int)e->y);

				// Prepare parameters for the selection tool...
				XMVECTOR rayOrigin, rayDir;
				//getPickingRay(x, y, rayOrigin, rayDir);		// TO-DO: SETTINGS()->camera->getPickingRay(clickedScreenCoords, clientWidth, clientHeight, rayOrigin, rayDir);

				POINT mouseCursorPoint;
				mouseCursorPoint.x = (LONG)clickedScreenCoords.x;
				mouseCursorPoint.y = (LONG)clickedScreenCoords.y;

				// Use the selection tool to select against objects in the scene and any present control handles for active transformation tools.
				currentlyChosenTransformTool->setActiveObject(SETTINGS()->selectedEnityId);
				//theSelectionTool->beginSelection(rayOrigin, rayDir, mCam, mScreenViewport, mouseCursorPoint, sceneObjects, currentlySelectedTransformationTool);
			}
		}

	case EVENT_MOUSE_MOVE:
		{
			Event_MouseMove* e = static_cast<Event_MouseMove*>(p_event);
			e->dx;

			Vector2 currentScreenCoords(e->x, e->y);

			if(currentlyChosenTransformTool && currentlyChosenTransformTool->getIsSelected())
			{
				// Prepare parameters for the transformation tool...
				XMVECTOR rayOrigin, rayDir;
				// getPickingRay(x, y, rayOrigin, rayDir);		// TO-DO: SETTINGS()->camera->getPickingRay(clickedScreenCoords, clientWidth, clientHeight, rayOrigin, rayDir);

				POINT mouseCursorPoint;
				mouseCursorPoint.x = (LONG)currentScreenCoords.x;
				mouseCursorPoint.y = (LONG)currentScreenCoords.y;

				//currentlyChosenTransformTool->update(rayOrigin, rayDir, mCam, mScreenViewport, mouseCursorPoint);
			}

			//if(theSelectionTool && theSelectionTool->getIsSelected())
			//{
			//	// Prepare parameters for the transformation tool...
			//	XMVECTOR rayOrigin, rayDir;
			//	getPickingRay(x, y, rayOrigin, rayDir);

			//	POINT mouseCursorPoint;
			//	mouseCursorPoint.x = x;
			//	mouseCursorPoint.y = y;

			//	theSelectionTool->update(rayOrigin, rayDir, mCam, mScreenViewport, mouseCursorPoint, sceneObjects);
			//}
		}
		break;
	case EVENT_MOUSE_RELEASE:
		{
			// If a translation tool is present and has been active, unselect it.
			if(currentlyChosenTransformTool && currentlyChosenTransformTool->getIsSelected())
			{
				currentlyChosenTransformTool->unselect();
			}

			//// Check if the selection tool is currently processing a selection.
			//if(theSelectionTool->getIsSelected())
			//{
			//	// Prepare parameters for the selection tool...
			//	XMVECTOR rayOrigin, rayDir;
			//	getPickingRay(x, y, rayOrigin, rayDir);

			//	POINT mouseCursorPoint;
			//	mouseCursorPoint.x = x;
			//	mouseCursorPoint.y = y;

			//	// Finalize the ongoing selection.
			//	theSelectionTool->finalizeSelection(rayOrigin, rayDir, mCam, mScreenViewport, mouseCursorPoint, sceneObjects);
			//}
		}

	default:
		break;
	}
}

Manager_3DTools::~Manager_3DTools()
{
	delete m_theTranslationTool;
}
