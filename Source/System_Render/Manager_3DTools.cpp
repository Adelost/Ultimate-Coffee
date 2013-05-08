#include "stdafx.h"
#include "Manager_3DTools.h"

#include <Core/Data.h>
#include <Core/DataMapper.h>
#include <Core/Events.h>
#include <Core/World.h>
#include <Core/Enums.h>

#include "ITool_Transformation.h"
#include "Tool_Translation.h"
#include "Tool_Scaling.h"
#include "Tool_Rotation.h"
#include "Tool_Selection.h"


Manager_3DTools::Manager_3DTools( ID3D11Device* p_device, ID3D11DeviceContext* p_deviceContext, ID3D11DepthStencilView* p_depthStencilView, D3D11_VIEWPORT *p_viewPort)
{
	m_deviceContext = p_deviceContext;
	m_depthStencilView = p_depthStencilView;

	SUBSCRIBE_TO_EVENT(this, EVENT_MOUSE_PRESS);
	SUBSCRIBE_TO_EVENT(this, EVENT_MOUSE_MOVE);
	SUBSCRIBE_TO_EVENT(this, EVENT_TRANSLATE_SCENE_ENTITY);
	//SUBSCRIBE_TO_EVENT(this, EVENT_ENTITY_SELECTION);

	// Initialize the transformation tools...
	currentlyChosenTransformTool = NULL;
	m_theSelectionTool = new Tool_Selection();

	m_theScalingTool = new Tool_Scaling();
	m_theRotationTool = new Tool_Rotation();
	m_theTranslationTool = new Tool_Translation();

	m_theScalingTool->init(p_device, p_deviceContext);
	m_theRotationTool->init(p_device, p_deviceContext);
	m_theTranslationTool->init(p_device, p_deviceContext);

	// HACK: Hard-coded the chosen transform tool here. To be chosen via toolbar and keyboard shortcuts.
	currentlyChosenTransformTool = m_theTranslationTool;
}

Manager_3DTools::~Manager_3DTools()
{
	delete m_theTranslationTool;
	delete m_theScalingTool;
	delete m_theRotationTool;
	delete m_theSelectionTool;
}


void Manager_3DTools::update()
{
	//// HACK (Mattias): Made the tool notice when selected Entity has been changed. Don't know if this is the best aproach.
	// NEW HACK (Nicolas): Took back my old hack for the time being.
	if(currentlyChosenTransformTool->getActiveObject() == -1)
		currentlyChosenTransformTool->setActiveObject(SETTINGS()->entity_selection->id());

	if(currentlyChosenTransformTool && currentlyChosenTransformTool->getActiveObject() != -1) // <- TEMP. ActiveEntity should be set via selection tool.
	{
		Entity* entity_camera = CAMERA_ENTITY().asEntity();
		Data::Transform* d_transform = entity_camera->fetchData<Data::Transform>();
		Data::Camera* d_camera = entity_camera->fetchData<Data::Camera>();

		XMFLOAT4X4 toolWorld = currentlyChosenTransformTool->getWorld_logical(); // Use the "logical world" if we don't want it to retain its size even whilst translating an object (could be distracting by giving a "mixed message" re: the object's actual location?)
		XMVECTOR origin = XMLoadFloat4(&XMFLOAT4(toolWorld._41, toolWorld._42, toolWorld._43, 1)); //XMLoadFloat4(&test_toolOrigo);
		XMVECTOR camPos = XMVectorSet(d_transform->position.x, d_transform->position.y, d_transform->position.z, 1.0f);

		float dist = XMVector3Length(XMVectorSubtract(camPos, origin)).m128_f32[0];
		float distanceAdjustedScale = dist / 6;
		currentlyChosenTransformTool->setScale(distanceAdjustedScale);

		if(currentlyChosenTransformTool == m_theTranslationTool)
			m_theTranslationTool->updateViewPlaneTranslationControlWorld(d_camera->look(), d_camera->up());
	}
}

void Manager_3DTools::draw(ID3D11DepthStencilView* p_depthStencilView)
{
	if(currentlyChosenTransformTool && currentlyChosenTransformTool->getActiveObject() != -1)
	{
		Entity* entity_camera = CAMERA_ENTITY().asEntity();
		Data::Transform* d_transform = entity_camera->fetchData<Data::Transform>();
		Data::Camera* d_camera = entity_camera->fetchData<Data::Camera>();

		//XMMATRIX foo = XMStoreFloat4x4(d_camera->view());
		XMMATRIX xm_camView = d_camera->view();
		XMMATRIX xm_camProj = d_camera->projection();
		currentlyChosenTransformTool->draw(xm_camView, xm_camProj, p_depthStencilView);
	}
}

void Manager_3DTools::onEvent( IEvent* p_event )
{
	EventType type = p_event->type();

	static Vector2 currentScreenCoords;

	switch (type) 
	{
	case EVENT_MOUSE_PRESS:
		{
			Event_MousePress* e = static_cast<Event_MousePress*>(p_event);

			Vector2 clickedScreenCoords((float)e->x, (float)e->y);

			if(e->isPressed == true)
			{
				if(e->keyEnum == Enum::QtKeyPress_MouseLeft)
				{
					// Prepare parameters for the selection tool...

					// Use the selection tool to select against objects in the scene and any present control handles for active transformation tools.
					currentlyChosenTransformTool->setActiveObject(SETTINGS()->entity_selection->id());

					Entity* entity_camera = CAMERA_ENTITY().asEntity();
					Data::Transform* d_transform = entity_camera->fetchData<Data::Transform>();
					Data::Camera* d_camera = entity_camera->fetchData<Data::Camera>();

					int height = SETTINGS()->windowSize.y; //m_viewPort->Height;
					int width = SETTINGS()->windowSize.x; //m_viewPort->Width;
					Vector2 screenDim((float)width, (float)height);
					Vector4 rayOrigin; Vector3 rayDir;
					d_camera->getPickingRay(clickedScreenCoords, screenDim, rayOrigin, rayDir);
					XMVECTOR xm_rayOrigin, xm_rayDir;
					xm_rayOrigin = rayOrigin; xm_rayDir = rayDir;

					XMMATRIX camView = d_camera->view();
					POINT mouseCursorPoint;
					mouseCursorPoint.x = (LONG)clickedScreenCoords.x;
					mouseCursorPoint.y = (LONG)clickedScreenCoords.y;
					m_theSelectionTool->beginSelection(xm_rayOrigin, xm_rayDir, camView, *m_viewPort, mouseCursorPoint, currentlyChosenTransformTool);
				}
			}
			else
			{
				// If a translation tool is present and has been active, unselect it.
				if(currentlyChosenTransformTool && currentlyChosenTransformTool->getIsSelected())
				{
					XMFLOAT4X4 test = currentlyChosenTransformTool->getWorld_logical();
					currentlyChosenTransformTool->unselect();

					// Prepare parameters for the hover test...
					Entity* entity_camera = CAMERA_ENTITY().asEntity();
					Data::Transform* d_transform = entity_camera->fetchData<Data::Transform>();
					Data::Camera* d_camera = entity_camera->fetchData<Data::Camera>();

					int height = SETTINGS()->windowSize.y; //m_viewPort->Height;
					int width = SETTINGS()->windowSize.x; //m_viewPort->Width;
					Vector2 screenDim((float)width, (float)height);
					Vector4 rayOrigin; Vector3 rayDir;
					d_camera->getPickingRay(currentScreenCoords, screenDim, rayOrigin, rayDir);
					XMVECTOR xm_rayOrigin, xm_rayDir;
					xm_rayOrigin = rayOrigin; xm_rayDir = rayDir;

					XMMATRIX camView = d_camera->view();
					XMMATRIX camProj = d_camera->projection();

					currentlyChosenTransformTool->tryForHover(xm_rayOrigin, xm_rayDir, camView);
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

			break;
		}
	case EVENT_MOUSE_MOVE:
		{
			Event_MouseMove* e = static_cast<Event_MouseMove*>(p_event);
			e->dx;
			
			currentScreenCoords = Vector2((float)e->x, (float)e->y);

			if(currentlyChosenTransformTool)
			{
				if(currentlyChosenTransformTool->getIsSelected())
				{
					// Prepare parameters for the transformation tool...
					Entity* entity_camera = CAMERA_ENTITY().asEntity();
					Data::Transform* d_transform = entity_camera->fetchData<Data::Transform>();
					Data::Camera* d_camera = entity_camera->fetchData<Data::Camera>();

					int height = SETTINGS()->windowSize.y; //m_viewPort->Height;
					int width = SETTINGS()->windowSize.x; //m_viewPort->Width;
					Vector2 screenDim((float)width, (float)height);
					Vector4 rayOrigin; Vector3 rayDir;
					d_camera->getPickingRay(currentScreenCoords, screenDim, rayOrigin, rayDir);
					XMVECTOR xm_rayOrigin, xm_rayDir;
					xm_rayOrigin = rayOrigin; xm_rayDir = rayDir;

					XMMATRIX camView = d_camera->view();
					XMMATRIX camProj = d_camera->projection();
					POINT mouseCursorPoint;
					mouseCursorPoint.x = (LONG)currentScreenCoords.x;
					mouseCursorPoint.y = (LONG)currentScreenCoords.y;
					currentlyChosenTransformTool->update(xm_rayOrigin, xm_rayDir, camView, camProj, *m_viewPort, mouseCursorPoint);
				}
				else if(!currentlyChosenTransformTool->getIsSelected())
				{
					// Prepare parameters for the hover test...
					Entity* entity_camera = CAMERA_ENTITY().asEntity();
					Data::Transform* d_transform = entity_camera->fetchData<Data::Transform>();
					Data::Camera* d_camera = entity_camera->fetchData<Data::Camera>();

					int height = SETTINGS()->windowSize.y; //m_viewPort->Height;
					int width = SETTINGS()->windowSize.x; //m_viewPort->Width;
					Vector2 screenDim((float)width, (float)height);
					Vector4 rayOrigin; Vector3 rayDir;
					d_camera->getPickingRay(currentScreenCoords, screenDim, rayOrigin, rayDir);
					XMVECTOR xm_rayOrigin, xm_rayDir;
					xm_rayOrigin = rayOrigin; xm_rayDir = rayDir;

					XMMATRIX camView = d_camera->view();
					XMMATRIX camProj = d_camera->projection();

					// See if what part of the tool we are hovering over.
					currentlyChosenTransformTool->tryForHover(xm_rayOrigin, xm_rayDir, camView);
				}
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

			break;
		}
	case EVENT_TRANSLATE_SCENE_ENTITY:
		{
			Event_TranslateSceneEntity* e = static_cast<Event_TranslateSceneEntity*>(p_event);

			Data::Transform* d_transform = Entity(e->m_idOfTranslatableSceneEntity).fetchData<Data::Transform>();

			d_transform->position.x = e->m_transX;
			d_transform->position.y = e->m_transY;
			d_transform->position.z = e->m_transZ;

			//if(currentlyChosenTransformTool->getActiveObject() == e->m_idOfTranslatableSceneEntity) // SELECTION HACK
			//	currentlyChosenTransformTool->updateWorld();

			// Gotta make sure to re-scale the visual component of the tool given the redoing/undoing changing the distance.

			break;
		}

	default:
		break;
	}
}

