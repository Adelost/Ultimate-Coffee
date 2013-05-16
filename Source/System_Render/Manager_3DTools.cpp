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
	SUBSCRIBE_TO_EVENT(this, EVENT_ROTATE_SCENE_ENTITY);
	SUBSCRIBE_TO_EVENT(this, EVENT_SCALE_SCENE_ENTITY);
	SUBSCRIBE_TO_EVENT(this, EVENT_ENTITY_SELECTION);
	SUBSCRIBE_TO_EVENT(this, EVENT_SET_TOOL);

	// Initialize the transformation tools...
	currentlyChosenTransformTool = NULL;
	
	m_theSelectionTool = NULL;

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
	//if(currentlyChosenTransformTool && currentlyChosenTransformTool->getActiveObject() == -1)
	//	currentlyChosenTransformTool->setActiveObject(SETTINGS()->entity_selection->id());

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
	DataMapper<Data::Selected> map_selected;
	bool thereIsAtLeastOneSelectedEntity = map_selected.hasNext();
	if(currentlyChosenTransformTool && thereIsAtLeastOneSelectedEntity && currentlyChosenTransformTool->getActiveObject() != -1)
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


//enum BoundaryType {BOUNDARY_TYPE_TRIANGLELIST = 0, BOUNDARY_TYPE_LINELIST, BOUNDARY_TYPE_BOX};
//
//class IBoundary;

//static bool doIntersectionTest(IBoundary *boundaryA, IBoundary *boundaryB)
//{
//	boundaryA
//
//	BoundaryType boundaryTypeA = boundaryA->getBoundaryType();
//	BoundaryType boundaryTypeB = boundaryB->getBoundaryType();
//
//	switch (boundaryTypeA)
//	{
//	case BOUNDARY_TYPE_TRIANGLELIST:
//
//		switch(boundaryTypeB)
//		{
//		case BOUNDARY_TYPE_TRIANGLELIST:
//			doIntersectionTest_TriangleListVsTriangleList(boundaryA->getBoundaryVertices(), boundaryB->getBoundaryVertices());
//			break;
//		case BOUNDARY_TYPE_LINELIST:
//			break;
//		case BOUNDARY_TYPE_BOX:
//			break;
//		}
//
//		break;
//	case BOUNDARY_TYPE_LINELIST:
//		break;
//	case BOUNDARY_TYPE_BOX:
//		break;
//	}
//}

void Manager_3DTools::onEvent( Event* p_event )
{
	EventType type = p_event->type();

	static Vector2 currentScreenCoords;

	DataMapper<Data::Selected> map_selected;
	bool thereIsAtLeastOneSelectedEntity = map_selected.hasNext();

	switch (type) 
	{
	case EVENT_MOUSE_PRESS:
		{

				Event_MousePress* e = static_cast<Event_MousePress*>(p_event);

				Vector2 clickedScreenCoords((float)e->x, (float)e->y);

				if(e->keyEnum == Enum::QtKeyPress_MouseLeft)
				{
					if(e->isPressed == true)
					{
						if(m_theSelectionTool)
						{
							// Prepare parameters for the selection tool...

							// Use the selection tool to select against objects in the scene and any present control handles for active transformation tools.
							//currentlyChosenTransformTool->setActiveObject(SETTINGS()->entity_selection->id());

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

							// Begin selecting with the selection tool.
							m_theSelectionTool->beginSelection(xm_rayOrigin, xm_rayDir, camView, *m_viewPort, mouseCursorPoint, currentlyChosenTransformTool);

							// If the chosen transform tool was selected at once, end the selecting.
							if(currentlyChosenTransformTool && currentlyChosenTransformTool->getIsSelected())
							{
								m_theSelectionTool->setIsSelecting(false);
							}
							else
								m_theSelectionTool->setIsSelecting(false); //m_theSelectionTool->setIsSelecting(true);
						}
					}
					else // Mouse left button up.
					{
						if(thereIsAtLeastOneSelectedEntity)
						{
							// If a translation tool is present and has been actived, unselect it.
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

								MyRectangle selectionRectangle;
								currentlyChosenTransformTool->tryForHover(selectionRectangle, xm_rayOrigin, xm_rayDir, camView);
							}
						}

						// Check if the selection tool is currently processing a selection, and, if so, finalize it.
						//if(m_theSelectionTool)
						//{
						//	if(m_theSelectionTool->getIsSelecting())
						//	{
						//		Entity* entity_camera = CAMERA_ENTITY().asEntity();
						//		Data::Transform* d_transform = entity_camera->fetchData<Data::Transform>();
						//		Data::Camera* d_camera = entity_camera->fetchData<Data::Camera>();

						//		int height = SETTINGS()->windowSize.y; //m_viewPort->Height;
						//		int width = SETTINGS()->windowSize.x; //m_viewPort->Width;
						//		Vector2 screenDim((float)width, (float)height);
						//		Vector4 rayOrigin; Vector3 rayDir;
						//		d_camera->getPickingRay(clickedScreenCoords, screenDim, rayOrigin, rayDir);
						//		XMVECTOR xm_rayOrigin, xm_rayDir;
						//		xm_rayOrigin = rayOrigin; xm_rayDir = rayDir;

						//		XMMATRIX camView = d_camera->view();
						//		POINT mouseCursorPoint;
						//		mouseCursorPoint.x = (LONG)clickedScreenCoords.x;
						//		mouseCursorPoint.y = (LONG)clickedScreenCoords.y;
			
						//		// Stop selecting, so the currently selected objects become the final selected objects.
						//		m_theSelectionTool->setIsSelecting(false);
						//		//m_theSelectionTool->finalizeSelection(xm_rayOrigin, xm_rayDir, camView, *m_viewPort, mouseCursorPoint);

						//		MyRectangle selectionRectangle = m_theSelectionTool->getSelectionRectangle();
						//	}
						//}
					}
	
				break;
			}
		}
	case EVENT_MOUSE_MOVE:
		{

				Event_MouseMove* e = static_cast<Event_MouseMove*>(p_event);
				e->dx;
			

				currentScreenCoords = Vector2((float)e->x, (float)e->y);
			
			if(thereIsAtLeastOneSelectedEntity)
			{
				// If a tool has been chosen...
				if(currentlyChosenTransformTool)
				{
					// ... and is selected, update its parameters.
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

						MyRectangle selectionRectangle;

						currentlyChosenTransformTool->update(selectionRectangle, xm_rayOrigin, xm_rayDir, camView, camProj, *m_viewPort, mouseCursorPoint);
					}
					// ... else, if a tool has been chosen but is not selected, and we're not in the process of selecting other things.
					else if(!currentlyChosenTransformTool->getIsSelected() && (m_theSelectionTool && !m_theSelectionTool->getIsSelecting()))
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
						MyRectangle selectionRectangle;
						currentlyChosenTransformTool->tryForHover(selectionRectangle, xm_rayOrigin, xm_rayDir, camView);
					}
				}
			}
		
				//// If the mouse is moving and we are in the process of selecting, update the parameters for the selection tool.
				//if(m_theSelectionTool)
				//{
				//	if(m_theSelectionTool->getIsSelecting())
				//	{
				//		// Prepare parameters for the transformation tool...
				//		Entity* entity_camera = CAMERA_ENTITY().asEntity();
				//		Data::Transform* d_transform = entity_camera->fetchData<Data::Transform>();
				//		Data::Camera* d_camera = entity_camera->fetchData<Data::Camera>();

				//		int height = SETTINGS()->windowSize.y; //m_viewPort->Height;
				//		int width = SETTINGS()->windowSize.x; //m_viewPort->Width;
				//		Vector2 screenDim((float)width, (float)height);
				//		Vector4 rayOrigin; Vector3 rayDir;
				//		d_camera->getPickingRay(currentScreenCoords, screenDim, rayOrigin, rayDir);
				//		XMVECTOR xm_rayOrigin, xm_rayDir;
				//		xm_rayOrigin = rayOrigin; xm_rayDir = rayDir;

				//		XMMATRIX camView = d_camera->view();
				//		XMMATRIX camProj = d_camera->projection();
				//		POINT mouseCursorPoint;
				//		mouseCursorPoint.x = (LONG)currentScreenCoords.x;
				//		mouseCursorPoint.y = (LONG)currentScreenCoords.y;

				//		MyRectangle selectionRectangle;

				//		m_theSelectionTool->update(xm_rayOrigin, xm_rayDir, camView, *m_viewPort, mouseCursorPoint);

				//		selectionRectangle = m_theSelectionTool->getSelectionRectangle();

				//		// Check for intersection against the selection rectangle / frustum
				//		// ...

				//		DataMapper<Data::Bounding> map_bounding;
				//		while(map_bounding.hasNext())
				//		{
				//			bool selectionRegionIntersectsWithEntity = false;

				//			Entity* entity = map_bounding.nextEntity();
				//			Data::Selected* d_selected = entity->fetchData<Data::Selected>();
				//	

				//			if(selectionRegionIntersectsWithEntity)
				//			{
				//				entity->addData(Data::Selected());
				//			}
				//		}
				//	}
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

			if(currentlyChosenTransformTool)
			{
				currentlyChosenTransformTool->setActiveObject(1);
			}

			break;
		}
	case EVENT_ROTATE_SCENE_ENTITY:
		{
			Event_RotateSceneEntity* e = static_cast<Event_RotateSceneEntity*>(p_event);

			Data::Transform* d_transform = Entity(e->m_idOfRotatableSceneEntity).fetchData<Data::Transform>();

			d_transform->rotation.x = e->m_quatX;
			d_transform->rotation.y = e->m_quatY;
			d_transform->rotation.z = e->m_quatZ;
			d_transform->rotation.w = e->m_quatW;

			if(currentlyChosenTransformTool)
			{
				currentlyChosenTransformTool->setActiveObject(1);
			}

			break;
		}
	case EVENT_SCALE_SCENE_ENTITY:
		{
			Event_ScaleSceneEntity* e = static_cast<Event_ScaleSceneEntity*>(p_event);

			Data::Transform* d_transform = Entity(e->m_idOfScalableSceneEntity).fetchData<Data::Transform>();

			d_transform->scale.x = e->m_scaleX;
			d_transform->scale.y = e->m_scaleY;
			d_transform->scale.z = e->m_scaleZ;

			if(currentlyChosenTransformTool)
			{
				currentlyChosenTransformTool->setActiveObject(1);
			}

			break;
		}
	case EVENT_ENTITY_SELECTION:
		{
			// Debug selection
// 			DEBUGPRINT("");
// 			DataMapper<Data::Selected> map_selected;
// 			if(Data::Selected::lastSelected.isValid())
// 				DEBUGPRINT("LAST_SELECTED: " + Converter::IntToStr(Data::Selected::lastSelected->id()));
// 			DEBUGPRINT("SELECTED");
// 			if(map_selected.dataCount() == 0)
// 				DEBUGPRINT(" NONE");
// 			while(map_selected.hasNext())
// 			{
// 				Entity* e = map_selected.nextEntity();
// 				DEBUGPRINT(" Entity: " + Converter::IntToStr(e->id()));
// 			}

			// Code goes here.
			if(currentlyChosenTransformTool)
			{
				currentlyChosenTransformTool->setActiveObject(1);
			}

			break;
		}
	case EVENT_SET_TOOL:
		{
			Event_SetTool* e = static_cast<Event_SetTool*>(p_event);

			switch(e->m_toolId)
			{
			case Enum::Tool_Translate:
				{
					currentlyChosenTransformTool = m_theTranslationTool;
					currentlyChosenTransformTool->setActiveObject(1);
				}
				break;
			case Enum::Tool_Rotate:
				{
					currentlyChosenTransformTool = m_theRotationTool;
					currentlyChosenTransformTool->setActiveObject(1);
				}
				break;
			case Enum::Tool_Scale:
				{
					currentlyChosenTransformTool = m_theScalingTool;
					currentlyChosenTransformTool->setActiveObject(1);
				}
				break; 
			default:
				break;
		}
		}
	}
}

