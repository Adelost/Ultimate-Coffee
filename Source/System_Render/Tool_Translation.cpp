#include "stdafx.h"
#include "Tool_Translation.h"

#include <Core/Data.h>
#include <Core/DataMapper.h>
#include <Core/Events.h>

#include <Core/Command_TranslateSceneEntity.h>

#include "GeometryGenerator.h"

Tool_Translation::Tool_Translation()
{
	isSelected = false;
	currentlySelectedAxis = NULL;
	currentlySelectedPlane = NULL;

	XMFLOAT3 xDir(1.0f, 0.0f, 0.0f);
	XMFLOAT3 yDir(0.0f, 1.0f, 0.0f);
	XMFLOAT3 zDir(0.0f, 0.0f, 1.0f);
	XMFLOAT3 zDirNeg(0.0f, 0.0f, -1.0f);

	MyRectangle boundingRectangle;

	boundingRectangle.P1 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	boundingRectangle.P2 = XMFLOAT3(0.0f, 1.0f, 0.0f);
	boundingRectangle.P3 = XMFLOAT3(0.0f, 1.0f, 1.0f);
	boundingRectangle.P4 = XMFLOAT3(0.0f, 0.0f, 1.0f);
	yzTranslationPlane = new Handle_TranslationPlane(XMLoadFloat3(&xDir), 0.0f, boundingRectangle);
		boundingRectangle.P1 = XMFLOAT3(0.0f, -1.0f, -1.0f);
		boundingRectangle.P2 = XMFLOAT3(0.0f,  0.0f, -1.0f);
		boundingRectangle.P3 = XMFLOAT3(0.0f,  0.0f,  0.0f);
		boundingRectangle.P4 = XMFLOAT3(0.0f, -1.0f,  0.0f);
		yzTranslationPlane2 = new Handle_TranslationPlane(-XMLoadFloat3(&xDir), 0.0f, boundingRectangle);

	boundingRectangle.P1 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	boundingRectangle.P2 = XMFLOAT3(0.0f, 0.0f, 1.0f);
	boundingRectangle.P3 = XMFLOAT3(1.0f, 0.0f, 1.0f);
	boundingRectangle.P4 = XMFLOAT3(1.0f, 0.0f, 0.0f);
	zxTranslationPlane = new Handle_TranslationPlane(XMLoadFloat3(&yDir), 0.0f, boundingRectangle);
		boundingRectangle.P1 = XMFLOAT3(-1.0f, 0.0f, -1.0f);
		boundingRectangle.P2 = XMFLOAT3(-1.0f, 0.0f,  0.0f);
		boundingRectangle.P3 = XMFLOAT3( 0.0f, 0.0f,  0.0f);
		boundingRectangle.P4 = XMFLOAT3( 0.0f, 0.0f, -1.0f);
		zxTranslationPlane2 = new Handle_TranslationPlane(-XMLoadFloat3(&yDir), 0.0f, boundingRectangle);

	boundingRectangle.P1 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	boundingRectangle.P2 = XMFLOAT3(0.0f, 1.0f, 0.0f);
	boundingRectangle.P3 = XMFLOAT3(1.0f, 1.0f, 0.0f);
	boundingRectangle.P4 = XMFLOAT3(1.0f, 0.0f, 0.0f);
	xyTranslationPlane = new Handle_TranslationPlane(XMLoadFloat3(&zDir), 0.0f, boundingRectangle);
		boundingRectangle.P1 = XMFLOAT3(-1.0f, -1.0f, 0.0f);
		boundingRectangle.P2 = XMFLOAT3(-1.0f,  0.0f, 0.0f);
		boundingRectangle.P3 = XMFLOAT3( 0.0f,  0.0f, 0.0f);
		boundingRectangle.P4 = XMFLOAT3( 0.0f, -1.0f, 0.0f);
		xyTranslationPlane2 = new Handle_TranslationPlane(-XMLoadFloat3(&zDir), 0.0f, boundingRectangle);

	boundingRectangle.P1 = XMFLOAT3(-0.25f, -0.25f, 0.0f);
	boundingRectangle.P2 = XMFLOAT3(-0.25f,  0.25f, 0.0f);
	boundingRectangle.P3 = XMFLOAT3( 0.25f,  0.25f, 0.0f);
	boundingRectangle.P4 = XMFLOAT3( 0.25f, -0.25f, 0.0f);
	camViewTranslationPlane = new Handle_TranslationPlane(-XMLoadFloat3(&zDir), 0.0f, boundingRectangle);
	
	relateToActiveObjectWorld = false;

	scale = 1.0f;
}

Tool_Translation::~Tool_Translation()
{
	delete xTranslationAxisHandle; delete xTranslationAxisHandle2;
	delete yTranslationAxisHandle; delete yTranslationAxisHandle2;
	delete zTranslationAxisHandle; delete zTranslationAxisHandle2;

	delete yzTranslationPlane; delete yzTranslationPlane2;
	delete zxTranslationPlane; delete zxTranslationPlane2;
	delete xyTranslationPlane; delete xyTranslationPlane2;
	delete camViewTranslationPlane;
	
	ReleaseCOM(mMeshTransTool_xAxisArrow_VB);
	ReleaseCOM(mMeshTransTool_yAxisArrow_VB);
	ReleaseCOM(mMeshTransTool_zAxisArrow_VB);
	ReleaseCOM(mMeshTransTool_yzPlane_VB);
	ReleaseCOM(mMeshTransTool_zxPlane_VB);
	ReleaseCOM(mMeshTransTool_xyPlane_VB);
	ReleaseCOM(mMeshTransTool_yzPlane2_VB);
	ReleaseCOM(mMeshTransTool_zxPlane2_VB);
	ReleaseCOM(mMeshTransTool_xyPlane2_VB);
	ReleaseCOM(mMeshTransTool_viewPlane_VB);
}

void Tool_Translation::setIsVisible(bool &isVisible)
{
	this->isVisible = isVisible;
}

/* Called for an instance of picking, possibly resulting in the tool being selected. */
bool Tool_Translation::tryForSelection(MyRectangle &selectionRectangle, XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, XMMATRIX &camProj, POINT &mouseCursorPoint)
{
	bool aTranslationToolHandleWasSelected = false;

	bool rayIntersectsWithToolBoundingBox = false;
	// Check if the picking ray intersects with any of the translation tool's bounding box.
	// ...
	// TEST: Pretend we hit it.
	rayIntersectsWithToolBoundingBox = true;

	currentlySelectedPlane = NULL;
	currentlySelectedAxis = NULL;

	if(activeEntity.isValid()) // Necessary add for the multi-trans functionality. Previously, updateWorld was repeatedly call during hack-y resettings of the active object, which is now only set once, with selection events.
		updateWorld();

	if(rayIntersectsWithToolBoundingBox)
	{
		// Check if the ray intersects with any of the control handles.
		
		float distanceToPointOfIntersection;
		
		bool arrowSelected = false;
		bool planeSelected = false;
		
		float distanceToClosestPointOfIntersection = FLT_MAX;

		distanceToClosestPointOfIntersection = FLT_MAX;

		arrowSelected = xTranslationAxisHandle->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, distanceToPointOfIntersection);
		if(arrowSelected)
		{
			distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
			currentlySelectedAxis = xTranslationAxisHandle;

			aTranslationToolHandleWasSelected = true;
		}

			arrowSelected = xTranslationAxisHandle2->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, distanceToPointOfIntersection);
			if(arrowSelected)
			{
				if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
				{
					distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
					currentlySelectedAxis = xTranslationAxisHandle2;

					aTranslationToolHandleWasSelected = true;
				}
			}

		arrowSelected = yTranslationAxisHandle->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, distanceToPointOfIntersection);
		if(arrowSelected)
		{
			if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
			{
				distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
				currentlySelectedAxis = yTranslationAxisHandle;

				aTranslationToolHandleWasSelected = true;
			}
		}

			arrowSelected = yTranslationAxisHandle2->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, distanceToPointOfIntersection);
			if(arrowSelected)
			{
				if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
				{
					distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
					currentlySelectedAxis = yTranslationAxisHandle2;

					aTranslationToolHandleWasSelected = true;
				}
			}

		arrowSelected = zTranslationAxisHandle->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, distanceToPointOfIntersection);
		if(arrowSelected)
		{
			if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
			{
				distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
				currentlySelectedAxis = zTranslationAxisHandle;

				aTranslationToolHandleWasSelected = true;
			}
		}

			arrowSelected = zTranslationAxisHandle2->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, distanceToPointOfIntersection);
			if(arrowSelected)
			{
				if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
				{
					distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
					currentlySelectedAxis = zTranslationAxisHandle2;

					aTranslationToolHandleWasSelected = true;
				}
			}

		if(!arrowSelected)
		{
			bool planeSelected = camViewTranslationPlane->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, distanceToPointOfIntersection);
			if(planeSelected)
			{
				float distanceToIntersectionPoint;
				XMMATRIX logicalWorld = XMLoadFloat4x4(&getWorld_logical());
				camViewTranslationPlane->setWorld(logicalWorld);
				camViewTranslationPlane->pickFirstPointOnPlane(rayOrigin, rayDir, camView, distanceToIntersectionPoint);

				currentlySelectedPlane = camViewTranslationPlane;
				aTranslationToolHandleWasSelected = true;
			}
		}
		
		if(!currentlySelectedPlane && !currentlySelectedAxis) // ... Else, the others are tested for intersection and prioritized by intersection point distance:
		{
			distanceToClosestPointOfIntersection = FLT_MAX;

			planeSelected = xyTranslationPlane->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, distanceToPointOfIntersection);
			if(planeSelected)
			{
				distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
				currentlySelectedPlane = xyTranslationPlane;
				aTranslationToolHandleWasSelected = true;
			}

				planeSelected = xyTranslationPlane2->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, distanceToPointOfIntersection);
				if(planeSelected)
				{
					if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
					{
						distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
						currentlySelectedPlane = xyTranslationPlane2;
						aTranslationToolHandleWasSelected = true;
					}
				}

			planeSelected = yzTranslationPlane->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, distanceToPointOfIntersection);
			if(planeSelected)
			{
				if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
				{
					distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
					currentlySelectedPlane = yzTranslationPlane;
					aTranslationToolHandleWasSelected = true;
				}
			}
			
				planeSelected = yzTranslationPlane2->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, distanceToPointOfIntersection);
				if(planeSelected)
				{
					if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
					{
						distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
						currentlySelectedPlane = yzTranslationPlane2;
						aTranslationToolHandleWasSelected = true;
					}
				}
				
			planeSelected = zxTranslationPlane->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, distanceToPointOfIntersection);
			if(planeSelected)
			{
				if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
				{
					distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
					currentlySelectedPlane = zxTranslationPlane;
					aTranslationToolHandleWasSelected = true;
				}
			}

				planeSelected = zxTranslationPlane2->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, distanceToPointOfIntersection);
				if(planeSelected)
				{
					if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
					{
						distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
						currentlySelectedPlane = zxTranslationPlane2;
						aTranslationToolHandleWasSelected = true;
					}
				}
		}
	}
	
	isSelected = aTranslationToolHandleWasSelected;

	//if(aTranslationToolHandleWasSelected)	// SELECTION HACK
	//{
	//	activeEntityId = SETTINGS()->selectedEntityId;

	//	// Set the visual and bounding components of the translation tool to the pivot point of the active object.
	//	updateWorld();

	//	XMMATRIX world = activeEntity->fetchData<Data::Transform>()->toWorldMatrix();

	//	XMStoreFloat4x4(&originalWorldOfActiveObject, world);
	//}

	return aTranslationToolHandleWasSelected;
}

void Tool_Translation::tryForHover(MyRectangle &selectionRectangle, XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView)
{
}

/* Called to bind the translatable object to the tool, so its translation can be modified. */
void Tool_Translation::setActiveObject(int entityId)
{
	//if(activeEntityId == SETTINGS()->selectedEntityId)	// SELECTIN HACK
	//{
	//	// Stay the same.
	//	int test = 3;
	//}
	//else
	//{
	//	activeEntityId = SETTINGS()->selectedEntityId;

	//	// Set the visual and bounding components of the translation tool to the pivot point of the active object.
	//	updateWorld();

	//	XMMATRIX world = activeEntity->fetchData<Data::Transform>()->toWorldMatrix();

	//	XMStoreFloat4x4(&originalWorldOfActiveObject, world);
	//}

	//DataMapper<Data::Selected> map_selected;
	//while(map_selected.hasNext())
	//{
	//Entity* e = map_selected.nextEntity();
	//e->removeData<Data::Selected>();
	//}

	//DataMapper<Data::Selected> map_selected;

	DataMapper<Data::Selected> map_selected;
	Entity* e;

	originalWorldsOfSelectedEntities.clear();

	bool thereIsAtLeastOneSelectedEntity = map_selected.hasNext();
	while(map_selected.hasNext())
	{
		e = map_selected.nextEntity();
		Data::Selected* d_selected = e->fetchData<Data::Selected>();

		XMMATRIX world = e->fetchData<Data::Transform>()->toWorldMatrix();
		XMFLOAT4X4 origWorld;
		XMStoreFloat4x4(&origWorld, world);
		originalWorldsOfSelectedEntities.push_back(origWorld);
	}

	if(thereIsAtLeastOneSelectedEntity && Data::Selected::lastSelected.isValid())
	{
		this->activeEntity = Data::Selected::lastSelected->toPointer();

		// Set the visual and bounding components of the translation tool to the pivot point of the active object.
		updateWorld();
	}
	else
	{
		activeEntity.invalidate();
	}
}

EntityPointer Tool_Translation::getActiveObject()
{
	return activeEntity;
}

void Tool_Translation::updateWorld()
{
	if(!relateToActiveObjectWorld && activeEntity.isValid())
	{
		// Just get the position of the active object, but keep the default orientation.
		Matrix newWorld = XMMatrixIdentity();

		XMFLOAT4X4 objectWorld;

		Data::Transform* trans = activeEntity->fetchData<Data::Transform>();
	
		//XMStoreFloat4x4(&objectWorld, activeObject->getIRenderable()->getWorld());
		
		newWorld._41 = trans->position.x; //objectWorld._41;
		newWorld._42 = trans->position.y; //objectWorld._42;
		newWorld._43 = trans->position.z; //objectWorld._43;

		// Update the translation tool's (distance-from-camera-adjusted) scale.
		//newWorld._11 = scale;
		//newWorld._22 = scale;
		//newWorld._33 = scale;
		//XMStoreFloat4x4(&world, newWorld);
		world = newWorld;

		XMMATRIX logicalWorld = XMLoadFloat4x4(&getWorld_logical());

		// Transform all the controls.
		xTranslationAxisHandle->setWorld(logicalWorld);
		yTranslationAxisHandle->setWorld(logicalWorld);
		zTranslationAxisHandle->setWorld(logicalWorld);
			xTranslationAxisHandle2->setWorld(logicalWorld);
			yTranslationAxisHandle2->setWorld(logicalWorld);
			zTranslationAxisHandle2->setWorld(logicalWorld);

		yzTranslationPlane->setWorld(logicalWorld);
		zxTranslationPlane->setWorld(logicalWorld);
		xyTranslationPlane->setWorld(logicalWorld);
			yzTranslationPlane2->setWorld(logicalWorld);
			zxTranslationPlane2->setWorld(logicalWorld);
			xyTranslationPlane2->setWorld(logicalWorld);

			//logicalWorld.r[0].m128_f32[0] *= 2.0f;
			//logicalWorld.r[1].m128_f32[1] *= 2.0f;
			//logicalWorld.r[2].m128_f32[2] *= 2.0f;
		camViewTranslationPlane->setWorld(XMLoadFloat4x4(&getWorld_viewPlaneTranslationControl_logical()));
		
		Data::Camera *camData = SETTINGS()->entity_camera->fetchData<Data::Camera>();
		XMVECTOR camLookVector = camData->getLookVector();
		camViewTranslationPlane->setPlaneOrientation(camLookVector);
	}
	else
	{
		world = activeEntity->fetchData<Data::Transform>()->toWorldMatrix();

		// Get the position and orientation of the active object.
		//XMStoreFloat4x4(&world, activeObject->getIRenderable()->getWorld());

		// Update the translation tool's (distance-from-camera-adjusted) scale.
		world._11 = scale;
		world._22 = scale;
		world._33 = scale;
		//XMStoreFloat4x4(&world, newWorld);

		XMMATRIX logicalWorld = XMLoadFloat4x4(&getWorld_logical());

		// Transform all the controls.
		xTranslationAxisHandle->setWorld(logicalWorld);
		yTranslationAxisHandle->setWorld(logicalWorld);
		zTranslationAxisHandle->setWorld(logicalWorld);
			xTranslationAxisHandle2->setWorld(logicalWorld);
			yTranslationAxisHandle2->setWorld(logicalWorld);
			zTranslationAxisHandle2->setWorld(logicalWorld);

		yzTranslationPlane->setWorld(logicalWorld);
		zxTranslationPlane->setWorld(logicalWorld);
		xyTranslationPlane->setWorld(logicalWorld);
			yzTranslationPlane2->setWorld(logicalWorld);
			zxTranslationPlane2->setWorld(logicalWorld);
			xyTranslationPlane2->setWorld(logicalWorld);
	}
}

/* Transform all controls to the local coord. sys. of the active object. */
void Tool_Translation::setRelateToActiveObjectWorld(bool relateToActiveObjectWorld)
{
	this->relateToActiveObjectWorld = relateToActiveObjectWorld;

	if(activeEntity.isValid())
		updateWorld();
}

/* Called to see if the translation tool is (still) active. */
bool Tool_Translation::getIsSelected()
{
	return isSelected;
}

/* Called to set the entity at whose pivot the tool is to be displayed, when a selection of one or more entities has been made. */
void Tool_Translation::setEntityAtWhosePivotTheToolIsToBeDisplayed(int entityId)
{
}

/* Called to send updated parameters to the translation tool, if it is still active. */
void Tool_Translation::update(MyRectangle &selectionRectangle, XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, XMMATRIX &camProj, D3D11_VIEWPORT &theViewport, POINT &mouseCursorPoint)
{
	XMVECTOR transDelta = XMVectorSet(0,0,0,0);
	if(currentlySelectedAxis)
	{
		//currentlySelectedAxis->update(rayOrigin, rayDir, camView);

		// Pick against the plane to update the translation delta.
		currentlySelectedAxis->pickAxisPlane(rayOrigin, rayDir, camView);

		transDelta = currentlySelectedAxis->getLastTranslationDelta();

		float scaleFactor = scale;
		Vector3 newTranslation;
		transDelta.m128_f32[0] = /*originalWorldOfActiveObject._41 +*/ transDelta.m128_f32[0] * scaleFactor;
		transDelta.m128_f32[1] = /*originalWorldOfActiveObject._42 +*/ transDelta.m128_f32[1] * scaleFactor;
		transDelta.m128_f32[2] = /*originalWorldOfActiveObject._43 +*/ transDelta.m128_f32[2] * scaleFactor;

		//Data::Transform* transform = activeEntity->fetchData<Data::Transform>();
		//transform->position = newTranslation;
	}
	else if(currentlySelectedPlane)
	{
		//if(currentlySelectedPlane == camViewTranslationPlane)
		//{
		//	updateWorld();
		//	Data::Camera *camData = SETTINGS()->entity_camera->fetchData<Data::Camera>();
		//	XMVECTOR camLookVector = camData->getLookVector();
		//	camViewTranslationPlane->setPlaneOrientation(-camLookVector);
		//}

		currentlySelectedPlane->pickPlane(rayOrigin, rayDir, camView);

		transDelta = currentlySelectedPlane->getLastTranslationDelta();

		float scaleFactor = scale;
		//if(currentlySelectedPlane == camViewTranslationPlane)
		//	scaleFactor *= 2.0f;

		Vector3 newTranslation;
		
		if(currentlySelectedPlane != camViewTranslationPlane)
		{
			transDelta.m128_f32[0] = transDelta.m128_f32[0] * scaleFactor; //newTranslation.x = originalWorldOfActiveObject._41 + transDelta.m128_f32[0] * scaleFactor;
			transDelta.m128_f32[1] = transDelta.m128_f32[1] * scaleFactor; //newTranslation.y = originalWorldOfActiveObject._42 + transDelta.m128_f32[1] * scaleFactor;
			transDelta.m128_f32[2] = transDelta.m128_f32[2] * scaleFactor; //newTranslation.z = originalWorldOfActiveObject._43 + transDelta.m128_f32[2] * scaleFactor;
		}
		else // Temp stuff until cam view translation works:
		{
			transDelta.m128_f32[0] = transDelta.m128_f32[0] * scaleFactor; //0.0f; //newTranslation.x = originalWorldOfActiveObject._41 + 0.0f;
			transDelta.m128_f32[1] = transDelta.m128_f32[1] * scaleFactor; //0.0f; //newTranslation.y = originalWorldOfActiveObject._42 + 0.0f;
			transDelta.m128_f32[2] = transDelta.m128_f32[2] * scaleFactor; //0.0f; //newTranslation.z = originalWorldOfActiveObject._43 + 0.0f;
		}

		//Data::Transform* transform = activeEntity->fetchData<Data::Transform>();
		//transform->position = newTranslation;
	}


		DataMapper<Data::Selected> map_selected;
		Entity* e;

		int i = 0;
		while(map_selected.hasNext())
		{
			e = map_selected.nextEntity();
			Data::Selected* d_selected = e->fetchData<Data::Selected>();

			e->fetchData<Data::Transform>()->position = XMVectorSet(originalWorldsOfSelectedEntities.at(i)._41,
																	originalWorldsOfSelectedEntities.at(i)._42,
																	originalWorldsOfSelectedEntities.at(i)._43, 1.0f) + transDelta;
			++i;
		}
	
}

/* Called when the translation tool is unselected, which makes any hitherto made translation final (and undoable). */
void Tool_Translation::unselect()
{
	// Set the controls' visual and bounding components to the active object's new position and orientation.
	
	
	//setActiveObject(1); //updateWorld();
	//updateWorld();

	if(currentlySelectedPlane)
	{
		currentlySelectedPlane->unselect();
		currentlySelectedPlane = NULL;
	}
	else if(currentlySelectedAxis)
	{
		currentlySelectedAxis->unselect();
		currentlySelectedAxis = NULL;
	}


	//Event_StoreCommandsAsSingleEntryInCommandHistoryGUI e2 = Event_StoreCommandsAsSingleEntryInCommandHistoryGUI(

	std::vector<Command*> translationCommands;
	DataMapper<Data::Selected> map_selected;
	Entity* e;
	unsigned int i = 0;
	while(map_selected.hasNext())
	{
		e = map_selected.nextEntity();

		Data::Transform* trans = e->fetchData<Data::Transform>();
		Command_TranslateSceneEntity *command = new Command_TranslateSceneEntity(e->id());
		command->setDoTranslation(trans->position.x, trans->position.y, trans->position.z);
		command->setUndoTranslation(originalWorldsOfSelectedEntities.at(i)._41, originalWorldsOfSelectedEntities.at(i)._42, originalWorldsOfSelectedEntities.at(i)._43);
		translationCommands.push_back(command);
		
		//SEND_EVENT(&Event_StoreCommandInCommandHistory(command, false));
		
		++i;
	}

	SEND_EVENT(&Event_StoreCommandsAsSingleEntryInCommandHistoryGUI(&translationCommands, false));

	setActiveObject(1);
	//activeEntityId = -1;

	isSelected = false;
}

XMFLOAT4X4 Tool_Translation::getWorld_logical()
{
	XMMATRIX trans = XMMatrixTranslation(world._41, world._42, world._43);

	XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);

	XMFLOAT4X4 world;
	XMStoreFloat4x4(&world, scaling * trans);

	return world;
}

XMFLOAT4X4 Tool_Translation::getWorld_visual()
{
	XMVECTOR trans = activeEntity->fetchData<Data::Transform>()->position;
	XMMATRIX translation = XMMatrixTranslationFromVector(trans);

	XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);

	XMFLOAT4X4 world_visual;
	XMStoreFloat4x4(&world_visual, scaling * translation);

	return world_visual;
}

void Tool_Translation::setScale(float &scale)
{
	this->scale = scale;
}

float Tool_Translation::getScale()
{
	return scale;
}

/* Called to see if the visual component of the translation tool should be rendered. */
bool Tool_Translation::getIsVisible()
{
	return isVisible;
}

void Tool_Translation::updateViewPlaneTranslationControlWorld(XMFLOAT3 &camViewVector, XMFLOAT3 &camUpVector, XMFLOAT3 &camRightVector)
{
	XMVECTOR loadedCamViewVector = XMLoadFloat3(&camViewVector);
	XMVECTOR loadedCamUpVector = XMLoadFloat3(&camUpVector);
	XMVECTOR loadedCamRightVector = XMLoadFloat3(&camRightVector); //XMVector3Cross(loadedCamUpVector, loadedCamViewVector);

	XMStoreFloat3((XMFLOAT3*)world_viewPlaneTranslationControl_logical.m[0], loadedCamRightVector);
	world_viewPlaneTranslationControl_logical.m[0][3] = 0.0f;
	XMStoreFloat3((XMFLOAT3*)world_viewPlaneTranslationControl_logical.m[1], loadedCamUpVector);
	world_viewPlaneTranslationControl_logical.m[1][3] = 0.0f;
	XMStoreFloat3((XMFLOAT3*)world_viewPlaneTranslationControl_logical.m[2], loadedCamViewVector);
	world_viewPlaneTranslationControl_logical.m[2][3] = 0.0f;

	Vector3 activeEntityPos = activeEntity->fetchData<Data::Transform>()->position;
	world_viewPlaneTranslationControl_logical._41 = activeEntityPos.x; //world.m[3][0];
	world_viewPlaneTranslationControl_logical._42 = activeEntityPos.y; //world.m[3][1];
	world_viewPlaneTranslationControl_logical._43 = activeEntityPos.z; //world.m[3][2];
	world_viewPlaneTranslationControl_logical._44 = 1.0f; //world.m[3][3];

	XMStoreFloat3((XMFLOAT3*)world_viewPlaneTranslationControl_visual.m[0], loadedCamRightVector);
	world_viewPlaneTranslationControl_visual.m[0][3] = 0.0f;
	XMStoreFloat3((XMFLOAT3*)world_viewPlaneTranslationControl_visual.m[1], loadedCamUpVector);
	world_viewPlaneTranslationControl_visual.m[1][3] = 0.0f;
	XMStoreFloat3((XMFLOAT3*)world_viewPlaneTranslationControl_visual.m[2], loadedCamViewVector);
	world_viewPlaneTranslationControl_visual.m[2][3] = 0.0f;

	world_viewPlaneTranslationControl_visual._41 = activeEntityPos.x;
	world_viewPlaneTranslationControl_visual._42 = activeEntityPos.y;
	world_viewPlaneTranslationControl_visual._43 = activeEntityPos.z;
	world_viewPlaneTranslationControl_visual._44 = 1.0f;
}

XMFLOAT4X4 Tool_Translation::getWorld_viewPlaneTranslationControl_logical()
{
	XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);

	XMMATRIX loadedWorld = XMLoadFloat4x4(&world_viewPlaneTranslationControl_logical);
	XMFLOAT4X4 logicalWorld;
	XMStoreFloat4x4(&logicalWorld, scaling * loadedWorld);

	return logicalWorld;
}

XMFLOAT4X4 Tool_Translation::getWorld_viewPlaneTranslationControl_visual()
{
	XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);

	XMMATRIX loadedWorld = XMLoadFloat4x4(&world_viewPlaneTranslationControl_visual);
	XMFLOAT4X4 visualWorld;
	XMStoreFloat4x4(&visualWorld, scaling * loadedWorld);

	return visualWorld;
}

void Tool_Translation::init(ID3D11Device *device, ID3D11DeviceContext *deviceContext)
{
	md3dDevice = device;
	md3dImmediateContext = deviceContext;

	ID3DBlob *PS_Buffer, *VS_Buffer;

	//hr = D3DReadFileToBlob(L"PixelShader.cso", &PS_Buffer);
	HR(D3DCompileFromFile(L"Tool_PS.hlsl", NULL, NULL, "PS", "ps_4_0", NULL, NULL, &PS_Buffer, NULL));
	HR(D3DCompileFromFile(L"Tool_VS.hlsl", NULL, NULL, "VS", "vs_4_0", NULL, NULL, &VS_Buffer, NULL));

	HR(md3dDevice->CreatePixelShader( PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &m_pixelShader));
	HR(md3dDevice->CreateVertexShader( VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &m_vertexShader));

	D3D11_INPUT_ELEMENT_DESC inputElementDesc [] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},	
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	HR(md3dDevice->CreateInputLayout(inputElementDesc, 2, VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), &m_inputLayout));

	ReleaseCOM(VS_Buffer);
	ReleaseCOM(PS_Buffer);

	D3D11_BUFFER_DESC WVP_Desc;
	ZeroMemory(&WVP_Desc, sizeof(WVP_Desc)); //memset(&WVP_Desc, 0, sizeof(WVP_Desc));
	WVP_Desc.Usage = D3D11_USAGE_DEFAULT;
	WVP_Desc.ByteWidth = 64;
	WVP_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	HR(md3dDevice->CreateBuffer(&WVP_Desc, NULL, &m_WVPBuffer));

	D3D11_BUFFER_DESC ColorSchemeId_Desc;
	ZeroMemory(&ColorSchemeId_Desc, sizeof(ColorSchemeId_Desc)); //memset(&WVP_Desc, 0, sizeof(WVP_Desc));
	ColorSchemeId_Desc.Usage = D3D11_USAGE_DEFAULT;
	ColorSchemeId_Desc.ByteWidth = 16;
	ColorSchemeId_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	HR(md3dDevice->CreateBuffer(&ColorSchemeId_Desc, NULL, &m_ColorSchemeIdBuffer));

	// Create test mesh for visual translation control.
	
	D3D11_BUFFER_DESC vbd;
	D3D11_SUBRESOURCE_DATA vinitData;

	std::vector<Vertex::PosCol> vertices;

	Vertex::PosCol posCol;

	// YZ line-list rectangle.

	posCol.Col.x = 1.0f; posCol.Col.y = 1.0f; posCol.Col.z = 0.0f;
	posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);

	posCol.Pos.x = 0.0f; posCol.Pos.y = 1.0f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);

	posCol.Col.x = 0.5f; posCol.Col.y = 1.0f; posCol.Col.z = 0.5f;
	posCol.Pos.x = 0.0f; posCol.Pos.y = 1.0f; posCol.Pos.z = 1.0f;
	vertices.push_back(posCol);

	posCol.Col.x = 0.0f; posCol.Col.y = 1.0f; posCol.Col.z = 1.0f;
	posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 1.0f;
	vertices.push_back(posCol);

	posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * 5;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransTool_yzPlane_VB));

	vertices.clear();

		posCol.Col.x = 1.0f; posCol.Col.y = 1.0f; posCol.Col.z = 0.0f;
		posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		posCol.Pos.x = 0.0f; posCol.Pos.y = -1.0f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		posCol.Col.x = 0.5f; posCol.Col.y = 1.0f; posCol.Col.z = 0.5f;
		posCol.Pos.x = 0.0f; posCol.Pos.y = -1.0f; posCol.Pos.z = -1.0f;
		vertices.push_back(posCol);

		posCol.Col.x = 0.0f; posCol.Col.y = 1.0f; posCol.Col.z = 1.0f;
		posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = -1.0f;
		vertices.push_back(posCol);

		posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex::PosCol) * 5;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vinitData.pSysMem = &vertices[0];
		HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransTool_yzPlane2_VB));

		vertices.clear();

	// ZX line-list rectangle.

	posCol.Col.x = 0.0f; posCol.Col.y = 1.0f; posCol.Col.z = 1.0f;
	posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);

	posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 1.0f;
	vertices.push_back(posCol);

	posCol.Col.x = 0.5f; posCol.Col.y = 0.5f; posCol.Col.z = 1.0f;
	posCol.Pos.x = 1.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 1.0f;
	vertices.push_back(posCol);

	posCol.Col.x = 1.0f; posCol.Col.y = 0.0f; posCol.Col.z = 1.0f;
	posCol.Pos.x = 1.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);
	
	posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * 5;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransTool_zxPlane_VB));

	vertices.clear();

		// ZX line-list rectangle 2.

		posCol.Col.x = 0.0f; posCol.Col.y = 1.0f; posCol.Col.z = 1.0f;
		posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = -1.0f;
		vertices.push_back(posCol);

		posCol.Col.x = 0.5f; posCol.Col.y = 0.5f; posCol.Col.z = 1.0f;
		posCol.Pos.x = -1.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = -1.0f;
		vertices.push_back(posCol);

		posCol.Col.x = 1.0f; posCol.Col.y = 0.0f; posCol.Col.z = 1.0f;
		posCol.Pos.x = -1.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);
	
		posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex::PosCol) * 5;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vinitData.pSysMem = &vertices[0];
		HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransTool_zxPlane2_VB));

		vertices.clear();
	

	// XY line-list rectangle.

	posCol.Col.x = 1.0f; posCol.Col.y = 1.0f; posCol.Col.z = 0.0f;
	posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);

	posCol.Pos.x = 0.0f; posCol.Pos.y = 1.0f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);

	posCol.Col.x = 1.0f; posCol.Col.y = 0.5f; posCol.Col.z = 0.5f;
	posCol.Pos.x = 1.0f; posCol.Pos.y = 1.0f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);

	posCol.Col.x = 1.0f; posCol.Col.y = 0.0f; posCol.Col.z = 1.0f;
	posCol.Pos.x = 1.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);

	posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * 5;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransTool_xyPlane_VB));

	vertices.clear();

	// XY line-list rectangle 2.

		posCol.Col.x = 1.0f; posCol.Col.y = 1.0f; posCol.Col.z = 0.0f;
		posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		posCol.Pos.x = 0.0f; posCol.Pos.y = -1.0f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		posCol.Col.x = 1.0f; posCol.Col.y = 0.5f; posCol.Col.z = 0.5f;
		posCol.Pos.x = -1.0f; posCol.Pos.y = -1.0f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		posCol.Col.x = 1.0f; posCol.Col.y = 0.0f; posCol.Col.z = 1.0f;
		posCol.Pos.x = -1.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex::PosCol) * 5;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vinitData.pSysMem = &vertices[0];
		HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransTool_xyPlane2_VB));

		vertices.clear();

		mMeshTransTool_xyPlane2_VB;

		// XY triangle-list rectangle.
		posCol.Col.x = 0.0f; posCol.Col.y = 0.0f; posCol.Col.z = 0.0f; posCol.Col.w = 0.0f; // Transparent.

		// Triangle A...

		posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f; //posCol.Pos.x = 0.009f; posCol.Pos.y = 0.009f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		posCol.Pos.x = 0.0f; posCol.Pos.y = 1.0f; posCol.Pos.z = 0.0f; // posCol.Pos.x = 0.009f; posCol.Pos.y = 0.99f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		posCol.Pos.x = 1.0f; posCol.Pos.y = 1.0f; posCol.Pos.z = 0.0f; // posCol.Pos.x = 0.99f; posCol.Pos.y = 0.99f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		// Triangle B...

		posCol.Pos.x = 1.0f; posCol.Pos.y = 1.0f; posCol.Pos.z = 0.0f; // posCol.Pos.x = 0.99f; posCol.Pos.y = 0.99f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		posCol.Pos.x = 1.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f; // posCol.Pos.x = 0.99f; posCol.Pos.y = 0.009f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f; // posCol.Pos.x = 0.009f; posCol.Pos.y = 0.009f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex::PosCol) * 6;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vinitData.pSysMem = &vertices[0];
		HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransTool_xyTriangleListRectangle_VB));

		vertices.clear();
	
	//mMeshTransTool_yzTriangleListRectangle_VB;
	//mMeshTransTool_zxTriangleListRectangle_VB;
	//mMeshTransTool_xyTriangleListRectangle_VB;
	//mMeshTransTool_viewPlaneTriangleListRectangle_VB;

	// View triangle-list rectangle.

	posCol.Col.x = 1.0f; posCol.Col.y = 1.0f; posCol.Col.z = 1.0f;
	posCol.Pos.x = -0.25f; posCol.Pos.y = -0.25f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);

	posCol.Col.x = 0.0f; posCol.Col.y = 0.0f; posCol.Col.z = 0.0f;
	posCol.Pos.x = -0.25f; posCol.Pos.y =  0.25f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);

	posCol.Col.x = 1.0f; posCol.Col.y = 1.0f; posCol.Col.z = 1.0f;
	posCol.Pos.x =  0.25f; posCol.Pos.y =  0.25f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);
	
	posCol.Col.x = 0.0f; posCol.Col.y = 0.0f; posCol.Col.z = 0.0f;
	posCol.Pos.x =  0.25f; posCol.Pos.y = -0.25f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);

	posCol.Col.x = 1.0f; posCol.Col.y = 1.0f; posCol.Col.z = 1.0f;
	posCol.Pos.x = -0.25f; posCol.Pos.y = -0.25f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * 5;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransTool_viewPlane_VB));

	vertices.clear();

	// Init the axis arrows.

	GeometryGenerator geoGen;

	XMVECTOR xAxisArrowColor = XMVectorSet(1.0f, 0.0f, 1.0f, 1.0f);
	XMVECTOR yAxisArrowColor = XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f);
	XMVECTOR zAxisArrowColor = XMVectorSet(0.0f, 1.0f, 1.0f, 1.0f);

	GeometryGenerator::MeshData2 meshVertices;

	// X arrows.
	XMFLOAT3 xDir(1.0f, 0.0f, 0.0f);
	XMFLOAT3 yDir(0.0f, 1.0f, 0.0f);
	XMFLOAT3 zDir(0.0f, 0.0f, 1.0f);

	float radianAngle = 90 * (Math::Pi / 180);
	float bottomRadius = 0.06735; //0.046875f; //0.03125f;
	XMMATRIX arrowLocalTransform;
	arrowLocalTransform = XMMatrixRotationZ(-radianAngle) * XMMatrixTranslation(1.0f, 0.0f, 0.0f);

	geoGen.CreateCylinder(bottomRadius, 0.0f, 0.5f, 10, 10, xAxisArrowColor, meshVertices, arrowLocalTransform);

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * meshVertices.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vinitData.pSysMem = &meshVertices.Vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransTool_xAxisArrow_VB));

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * meshVertices.Indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    vinitData.pSysMem = &meshVertices.Indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &vinitData, &mMeshTransTool_axisArrow_IB));

	// Record bounding triangles for the handle by creating a proper trianglelist from the indices.
	
	std::vector<XMFLOAT4> listOfTrianglesAsPoints;
	listOfTrianglesAsPoints.resize(0);
	//ZeroMemory(&listOfTrianglesAsPoints, sizeof(std::vector<XMFLOAT4>));

	for(unsigned int i = 0; i < meshVertices.Indices.size(); i = i + 3)
	{
		unsigned int indexA = meshVertices.Indices.at(i);
		unsigned int indexB = meshVertices.Indices.at(i + 1);
		unsigned int indexC = meshVertices.Indices.at(i + 2);

		XMFLOAT4 trianglePointA = XMFLOAT4(meshVertices.Vertices.at(indexA).Position.x, meshVertices.Vertices.at(indexA).Position.y, meshVertices.Vertices.at(indexA).Position.z, 1.0f);
		XMFLOAT4 trianglePointB = XMFLOAT4(meshVertices.Vertices.at(indexB).Position.x, meshVertices.Vertices.at(indexB).Position.y, meshVertices.Vertices.at(indexB).Position.z, 1.0f);
		XMFLOAT4 trianglePointC = XMFLOAT4(meshVertices.Vertices.at(indexC).Position.x, meshVertices.Vertices.at(indexC).Position.y, meshVertices.Vertices.at(indexC).Position.z, 1.0f);

		listOfTrianglesAsPoints.push_back(trianglePointA);
		listOfTrianglesAsPoints.push_back(trianglePointB);
		listOfTrianglesAsPoints.push_back(trianglePointC);
	}

	xTranslationAxisHandle = new Handle_TranslationAxis(XMLoadFloat3(&xDir), listOfTrianglesAsPoints, 'x');
	listOfTrianglesAsPoints.clear();

	meshVertices.Vertices.clear();

		arrowLocalTransform = XMMatrixRotationZ(radianAngle) * XMMatrixTranslation(-1.0f, 0.0f, 0.0f);

		geoGen.CreateCylinder(bottomRadius, 0.0f, 0.5f, 10, 10, xAxisArrowColor, meshVertices, arrowLocalTransform);

		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex::PosCol) * meshVertices.Vertices.size();
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vinitData.pSysMem = &meshVertices.Vertices[0];
		HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransTool_xAxisArrow2_VB));

		// Record bounding triangles for the handle by creating a proper trianglelist from the indices.
		for(unsigned int i = 0; i < meshVertices.Indices.size(); i = i + 3)
		{
			unsigned int indexA = meshVertices.Indices.at(i);
			unsigned int indexB = meshVertices.Indices.at(i + 1);
			unsigned int indexC = meshVertices.Indices.at(i + 2);

			XMFLOAT4 trianglePointA = XMFLOAT4(meshVertices.Vertices.at(indexA).Position.x, meshVertices.Vertices.at(indexA).Position.y, meshVertices.Vertices.at(indexA).Position.z, 1.0f);
			XMFLOAT4 trianglePointB = XMFLOAT4(meshVertices.Vertices.at(indexB).Position.x, meshVertices.Vertices.at(indexB).Position.y, meshVertices.Vertices.at(indexB).Position.z, 1.0f);
			XMFLOAT4 trianglePointC = XMFLOAT4(meshVertices.Vertices.at(indexC).Position.x, meshVertices.Vertices.at(indexC).Position.y, meshVertices.Vertices.at(indexC).Position.z, 1.0f);

			listOfTrianglesAsPoints.push_back(trianglePointA);
			listOfTrianglesAsPoints.push_back(trianglePointB);
			listOfTrianglesAsPoints.push_back(trianglePointC);
		}

		xTranslationAxisHandle2 = new Handle_TranslationAxis(XMLoadFloat3(&xDir), listOfTrianglesAsPoints, 'x');
		listOfTrianglesAsPoints.clear();

		meshVertices.Vertices.clear();

	// Y arrow.
	
	arrowLocalTransform = XMMatrixTranslation(0.0f, 1.0f, 0.0f);

	geoGen.CreateCylinder(bottomRadius, 0.0f, 0.5f, 10, 10, yAxisArrowColor, meshVertices, arrowLocalTransform);

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * meshVertices.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vinitData.pSysMem = &meshVertices.Vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransTool_yAxisArrow_VB));

	// Record bounding triangles for the handle by creating a proper trianglelist from the indices.
	for(unsigned int i = 0; i < meshVertices.Indices.size(); i = i + 3)
	{
		unsigned int indexA = meshVertices.Indices.at(i);
		unsigned int indexB = meshVertices.Indices.at(i + 1);
		unsigned int indexC = meshVertices.Indices.at(i + 2);

		XMFLOAT4 trianglePointA = XMFLOAT4(meshVertices.Vertices.at(indexA).Position.x, meshVertices.Vertices.at(indexA).Position.y, meshVertices.Vertices.at(indexA).Position.z, 1.0f);
		XMFLOAT4 trianglePointB = XMFLOAT4(meshVertices.Vertices.at(indexB).Position.x, meshVertices.Vertices.at(indexB).Position.y, meshVertices.Vertices.at(indexB).Position.z, 1.0f);
		XMFLOAT4 trianglePointC = XMFLOAT4(meshVertices.Vertices.at(indexC).Position.x, meshVertices.Vertices.at(indexC).Position.y, meshVertices.Vertices.at(indexC).Position.z, 1.0f);

		listOfTrianglesAsPoints.push_back(trianglePointA);
		listOfTrianglesAsPoints.push_back(trianglePointB);
		listOfTrianglesAsPoints.push_back(trianglePointC);
	}

	yTranslationAxisHandle = new Handle_TranslationAxis(XMLoadFloat3(&yDir), listOfTrianglesAsPoints, 'y');
	listOfTrianglesAsPoints.clear();

	meshVertices.Vertices.clear();

		arrowLocalTransform = XMMatrixRotationX(2 * radianAngle) * XMMatrixTranslation(0.0f, -1.0f, 0.0f);

		geoGen.CreateCylinder(bottomRadius, 0.0f, 0.5f, 10, 10, yAxisArrowColor, meshVertices, arrowLocalTransform);

		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex::PosCol) * meshVertices.Vertices.size();
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vinitData.pSysMem = &meshVertices.Vertices[0];
		HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransTool_yAxisArrow2_VB));

		// Record bounding triangles for the handle by creating a proper trianglelist from the indices.
		for(unsigned int i = 0; i < meshVertices.Indices.size(); i = i + 3)
		{
			unsigned int indexA = meshVertices.Indices.at(i);
			unsigned int indexB = meshVertices.Indices.at(i + 1);
			unsigned int indexC = meshVertices.Indices.at(i + 2);

			XMFLOAT4 trianglePointA = XMFLOAT4(meshVertices.Vertices.at(indexA).Position.x, meshVertices.Vertices.at(indexA).Position.y, meshVertices.Vertices.at(indexA).Position.z, 1.0f);
			XMFLOAT4 trianglePointB = XMFLOAT4(meshVertices.Vertices.at(indexB).Position.x, meshVertices.Vertices.at(indexB).Position.y, meshVertices.Vertices.at(indexB).Position.z, 1.0f);
			XMFLOAT4 trianglePointC = XMFLOAT4(meshVertices.Vertices.at(indexC).Position.x, meshVertices.Vertices.at(indexC).Position.y, meshVertices.Vertices.at(indexC).Position.z, 1.0f);

			listOfTrianglesAsPoints.push_back(trianglePointA);
			listOfTrianglesAsPoints.push_back(trianglePointB);
			listOfTrianglesAsPoints.push_back(trianglePointC);
		}

		yTranslationAxisHandle2 = new Handle_TranslationAxis(XMLoadFloat3(&yDir), listOfTrianglesAsPoints, 'y');
		listOfTrianglesAsPoints.clear();

		meshVertices.Vertices.clear();

	// Z arrow.

	arrowLocalTransform = XMMatrixRotationX(radianAngle) * XMMatrixTranslation(0.0f, 0.0f, 1.0f);

	geoGen.CreateCylinder(bottomRadius, 0.0f, 0.5f, 10, 10, zAxisArrowColor, meshVertices, arrowLocalTransform);

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * meshVertices.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vinitData.pSysMem = &meshVertices.Vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransTool_zAxisArrow_VB));

	// Record bounding triangles for the handle by creating a proper trianglelist from the indices.
	for(unsigned int i = 0; i < meshVertices.Indices.size(); i = i + 3)
	{
		unsigned int indexA = meshVertices.Indices.at(i);
		unsigned int indexB = meshVertices.Indices.at(i + 1);
		unsigned int indexC = meshVertices.Indices.at(i + 2);

		XMFLOAT4 trianglePointA = XMFLOAT4(meshVertices.Vertices.at(indexA).Position.x, meshVertices.Vertices.at(indexA).Position.y, meshVertices.Vertices.at(indexA).Position.z, 1.0f);
		XMFLOAT4 trianglePointB = XMFLOAT4(meshVertices.Vertices.at(indexB).Position.x, meshVertices.Vertices.at(indexB).Position.y, meshVertices.Vertices.at(indexB).Position.z, 1.0f);
		XMFLOAT4 trianglePointC = XMFLOAT4(meshVertices.Vertices.at(indexC).Position.x, meshVertices.Vertices.at(indexC).Position.y, meshVertices.Vertices.at(indexC).Position.z, 1.0f);

		listOfTrianglesAsPoints.push_back(trianglePointA);
		listOfTrianglesAsPoints.push_back(trianglePointB);
		listOfTrianglesAsPoints.push_back(trianglePointC);
	}

	zTranslationAxisHandle = new Handle_TranslationAxis(XMLoadFloat3(&zDir), listOfTrianglesAsPoints, 'z');
	listOfTrianglesAsPoints.clear();

	meshVertices.Vertices.clear();

		arrowLocalTransform = XMMatrixRotationX(-radianAngle) * XMMatrixTranslation(0.0f, 0.0f, -1.0f);

		geoGen.CreateCylinder(bottomRadius, 0.0f, 0.5f, 10, 10, zAxisArrowColor, meshVertices, arrowLocalTransform);

		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex::PosCol) * meshVertices.Vertices.size();
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vinitData.pSysMem = &meshVertices.Vertices[0];
		HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransTool_zAxisArrow2_VB));

		// Record bounding triangles for the handle by creating a proper trianglelist from the indices.
		for(unsigned int i = 0; i < meshVertices.Indices.size(); i = i + 3)
		{
			unsigned int indexA = meshVertices.Indices.at(i);
			unsigned int indexB = meshVertices.Indices.at(i + 1);
			unsigned int indexC = meshVertices.Indices.at(i + 2);

			XMFLOAT4 trianglePointA = XMFLOAT4(meshVertices.Vertices.at(indexA).Position.x, meshVertices.Vertices.at(indexA).Position.y, meshVertices.Vertices.at(indexA).Position.z, 1.0f);
			XMFLOAT4 trianglePointB = XMFLOAT4(meshVertices.Vertices.at(indexB).Position.x, meshVertices.Vertices.at(indexB).Position.y, meshVertices.Vertices.at(indexB).Position.z, 1.0f);
			XMFLOAT4 trianglePointC = XMFLOAT4(meshVertices.Vertices.at(indexC).Position.x, meshVertices.Vertices.at(indexC).Position.y, meshVertices.Vertices.at(indexC).Position.z, 1.0f);

			listOfTrianglesAsPoints.push_back(trianglePointA);
			listOfTrianglesAsPoints.push_back(trianglePointB);
			listOfTrianglesAsPoints.push_back(trianglePointC);
		}

		zTranslationAxisHandle2 = new Handle_TranslationAxis(XMLoadFloat3(&zDir), listOfTrianglesAsPoints, 'z');
		listOfTrianglesAsPoints.clear();

		meshVertices.Vertices.clear();
		meshVertices.Indices.clear();

	// Init guiding lines...

	float lengthOfLine = 3000.0f;
	XMMATRIX localSpaceTrans;
	XMVECTOR endPointA;
	XMVECTOR endPointB;
	XMVECTOR colorA;
	XMVECTOR colorB;

	// X guiding line.

	localSpaceTrans = XMMatrixIdentity();
	endPointA = XMVectorSet(-lengthOfLine, 0.0f, 0.0f, 1.0f);
	endPointB = XMVectorSet(lengthOfLine, 0.0f, 0.0f, 1.0f);
	colorA = XMVectorSet(1.0f, 0.0f, 1.0f, 1.0f);
	colorB = XMVectorSet(1.0f, 0.0f, 1.0f, 1.0f);
	geoGen.createLine(endPointA, endPointB, 0, colorA, colorB, localSpaceTrans, meshVertices);

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * meshVertices.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vinitData.pSysMem = &meshVertices.Vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransTool_xAxisLine_VB));

	meshVertices.Vertices.clear();

	// Y guiding line.

	localSpaceTrans = XMMatrixRotationZ(-Math::Pi / 2);
	endPointA = XMVectorSet(-lengthOfLine, 0.0f, 0.0f, 1.0f);
	endPointB = XMVectorSet(+lengthOfLine, 0.0f, 0.0f, 1.0f);
	colorA = XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f);
	colorB = XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f);
	geoGen.createLine(endPointA, endPointB, 0, colorA, colorB, localSpaceTrans, meshVertices);

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * meshVertices.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vinitData.pSysMem = &meshVertices.Vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransTool_yAxisLine_VB));

	meshVertices.Vertices.clear();

	// Z guiding line.

	localSpaceTrans = XMMatrixRotationY(Math::Pi / 2);
	endPointA = XMVectorSet(-lengthOfLine, 0.0f, 0.0f, 1.0f);
	endPointB = XMVectorSet(+lengthOfLine, 0.0f, 0.0f, 1.0f);
	colorA = XMVectorSet(0.0f, 1.0f, 1.0f, 1.0f);
	colorB = XMVectorSet(0.0f, 1.0f, 1.0f, 1.0f);
	geoGen.createLine(endPointA, endPointB, 0, colorA, colorB, localSpaceTrans, meshVertices);

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * meshVertices.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vinitData.pSysMem = &meshVertices.Vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransTool_zAxisLine_VB));

	meshVertices.Vertices.clear();
}

void Tool_Translation::draw(XMMATRIX &camView, XMMATRIX &camProj, ID3D11DepthStencilView *depthStencilView)
{
	// Draw the translation tool...
	md3dImmediateContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	md3dImmediateContext->PSSetShader(m_pixelShader, 0, 0);
	md3dImmediateContext->VSSetShader(m_vertexShader, 0, 0);

	md3dImmediateContext->IASetInputLayout(m_inputLayout);
   
	UINT stride = sizeof(Vertex::PosCol);
    UINT offset = 0;

	XMVECTOR rotQuat = activeEntity->fetchData<Data::Transform>()->rotation;;
	XMMATRIX rotation = XMMatrixRotationQuaternion(rotQuat);

	XMFLOAT4X4 toolWorld = getWorld_visual();
	XMMATRIX world = XMLoadFloat4x4(&toolWorld);
	
	XMMATRIX worldViewProj = world * camView * camProj;
	worldViewProj = XMMatrixTranspose(worldViewProj);

	//ConstantBuffer2 WVP;
	//WVP.WVP = worldViewProj;
	//md3dImmediateContext->UpdateSubresource(m_WVPBuffer, 0, NULL, &WVP, 0, 0);
	//md3dImmediateContext->VSSetConstantBuffers(0, 1, &m_WVPBuffer);

	md3dImmediateContext->UpdateSubresource(m_WVPBuffer, 0, NULL, &worldViewProj, 0, 0);
	md3dImmediateContext->UpdateSubresource(m_ColorSchemeIdBuffer, 0, NULL, &SETTINGS()->colorScheme, 0, 0);
	ID3D11Buffer *buffers[2] = {m_WVPBuffer, m_ColorSchemeIdBuffer};
	md3dImmediateContext->VSSetConstantBuffers(0, 2, buffers);

	// Draw control frames.

				//md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			
				//D3D11_BLEND_DESC blendDesc;
				//blendDesc.AlphaToCoverageEnable = false;
				//blendDesc.IndependentBlendEnable = false;
				//blendDesc.RenderTarget[0].BlendEnable = true;
				//blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;
				//blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_BLEND_FACTOR;
				//blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				//blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				//blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				//blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				//blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			
				//ID3D11BlendState *blendState;
				//md3dDevice->CreateBlendState(&blendDesc, &blendState);
			
				//float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				//UINT sampleMask   = 0xffffffff;
			
				//md3dImmediateContext->OMSetBlendState(blendState, NULL, sampleMask);
				//md3dImmediateContext->RSSetState(RenderStates::DepthBiasedRS);
			
				//ReleaseCOM(blendState);
			
				//md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_xyTriangleListRectangle_VB, &stride, &offset);
				//md3dImmediateContext->Draw(6, 0);
			
				//md3dImmediateContext->OMSetBlendState(NULL, blendFactor, sampleMask);

	//md3dImmediateContext->OMSetDepthStencilState(RenderStates::LessEqualDSS, 0);



	if(!isSelected || currentlySelectedPlane == yzTranslationPlane || currentlySelectedPlane == yzTranslationPlane2)
	{
		md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_yzPlane_VB, &stride, &offset);
		md3dImmediateContext->Draw(5, 0);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_yzPlane2_VB, &stride, &offset);
		md3dImmediateContext->Draw(5, 0);

		if(isSelected)
		{
			// Draw guiding lines.
			md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

			md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_yAxisLine_VB, &stride, &offset);
			md3dImmediateContext->Draw(2, 0);

			md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_zAxisLine_VB, &stride, &offset);
			md3dImmediateContext->Draw(2, 0);
		}
	}


	if(!isSelected || currentlySelectedPlane == zxTranslationPlane || currentlySelectedPlane == zxTranslationPlane2)
	{
		md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_zxPlane_VB, &stride, &offset);
		md3dImmediateContext->Draw(5, 0);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_zxPlane2_VB, &stride, &offset);
		md3dImmediateContext->Draw(5, 0);

		if(isSelected)
		{
			// Draw guiding lines.
			md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

			md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_xAxisLine_VB, &stride, &offset);
			md3dImmediateContext->Draw(2, 0);

			md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_zAxisLine_VB, &stride, &offset);
			md3dImmediateContext->Draw(2, 0);
		}
	}

	if(!isSelected || currentlySelectedPlane == xyTranslationPlane || currentlySelectedPlane == xyTranslationPlane2)
	{
		md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_xyPlane_VB, &stride, &offset);
		md3dImmediateContext->Draw(5, 0);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_xyPlane2_VB, &stride, &offset);
		md3dImmediateContext->Draw(5, 0);

		if(isSelected)
		{
			// Draw guiding lines.
			md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

			md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_xAxisLine_VB, &stride, &offset);
			md3dImmediateContext->Draw(2, 0);

			md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_yAxisLine_VB, &stride, &offset);
			md3dImmediateContext->Draw(2, 0);
		}
	}

	// Draw arrows.

	md3dImmediateContext->IASetIndexBuffer(mMeshTransTool_axisArrow_IB, DXGI_FORMAT_R32_UINT, offset);

	if(!isSelected || currentlySelectedAxis == xTranslationAxisHandle
				   || currentlySelectedAxis == xTranslationAxisHandle2
				   || currentlySelectedPlane == xyTranslationPlane
				   || currentlySelectedPlane == xyTranslationPlane2
				   || currentlySelectedPlane == zxTranslationPlane
				   || currentlySelectedPlane == zxTranslationPlane2)
	{
		md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_xAxisArrow_VB, &stride, &offset);
		md3dImmediateContext->DrawIndexed(660, 0, 0);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_xAxisArrow2_VB, &stride, &offset);
		md3dImmediateContext->DrawIndexed(660, 0, 0);

		if(isSelected)
		{
			// Draw guiding lines.
			md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

			md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_xAxisLine_VB, &stride, &offset);
			md3dImmediateContext->Draw(2, 0);
		}
	}

	if(!isSelected || currentlySelectedAxis == yTranslationAxisHandle
				   || currentlySelectedAxis == yTranslationAxisHandle2
				   || currentlySelectedPlane == yzTranslationPlane
				   || currentlySelectedPlane == yzTranslationPlane2
				   || currentlySelectedPlane == xyTranslationPlane
				   || currentlySelectedPlane == xyTranslationPlane2)
	{
		md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_yAxisArrow_VB, &stride, &offset);
		md3dImmediateContext->DrawIndexed(660, 0, 0);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_yAxisArrow2_VB, &stride, &offset);
		md3dImmediateContext->DrawIndexed(660, 0, 0);

		if(isSelected)
		{
			// Draw guiding lines.
			md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

			md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_yAxisLine_VB, &stride, &offset);
			md3dImmediateContext->Draw(2, 0);
		}
	}

	if(!isSelected || currentlySelectedAxis == zTranslationAxisHandle
				   || currentlySelectedAxis == zTranslationAxisHandle2
				   || currentlySelectedPlane == zxTranslationPlane
				   || currentlySelectedPlane == zxTranslationPlane2
				   || currentlySelectedPlane == yzTranslationPlane
				   || currentlySelectedPlane == yzTranslationPlane2)
	{
		md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_zAxisArrow_VB, &stride, &offset);
		md3dImmediateContext->DrawIndexed(660, 0, 0);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_zAxisArrow2_VB, &stride, &offset);
		md3dImmediateContext->DrawIndexed(660, 0, 0);

		if(isSelected)
		{
			// Draw guiding lines.
			md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

			md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_zAxisLine_VB, &stride, &offset);
			md3dImmediateContext->Draw(2, 0);
		}
	}
	
	if(!isSelected || currentlySelectedPlane == camViewTranslationPlane)
	{
		md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

		md3dImmediateContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		//md3dImmediateContext->OMSetDepthStencilState(RenderStates::GreaterEqualDSS, 0);

		XMMATRIX viewControlWorld = XMLoadFloat4x4(&getWorld_viewPlaneTranslationControl_visual());

		float scale = getScale();
		XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);

		worldViewProj = viewControlWorld * camView * camProj;
		worldViewProj = XMMatrixTranspose(worldViewProj);

		md3dImmediateContext->UpdateSubresource(m_WVPBuffer, 0, NULL, &worldViewProj, 0, 0);
		md3dImmediateContext->VSSetConstantBuffers(0, 1, &m_WVPBuffer);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_viewPlane_VB, &stride, &offset);
		md3dImmediateContext->Draw(5, 0);
	}
}
