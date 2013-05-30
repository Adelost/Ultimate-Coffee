#include "stdafx.h"
#include "Tool_Scaling.h"
#include "GeometryGenerator.h"

#include <Core/Data.h>
#include <Core/DataMapper.h>
#include <Core/Events.h>

#include <Core/Command_ScaleSceneEntity.h>

Tool_Scaling::Tool_Scaling(/*HWND windowHandle*/)
{
	isSelected = false;
	currentlySelectedAxis = NULL;
	currentlySelectedPlane = NULL;
	currentlySelectedHandle = NULL;

	XMFLOAT3 xDir(1.0f, 0.0f, 0.0f);
	XMFLOAT3 yDir(0.0f, 1.0f, 0.0f);
	XMFLOAT3 zDir(0.0f, 0.0f, 1.0f);
	XMFLOAT3 zDirNeg(0.0f, 0.0f, -1.0f);

	//xTranslationAxisHandle = new Handle_TranslationAxis(XMLoadFloat3(&xDir));
	//yTranslationAxisHandle = new Handle_TranslationAxis(XMLoadFloat3(&yDir));
	//zTranslationAxisHandle = new Handle_TranslationAxis(XMLoadFloat3(&zDir));

	MyRectangle boundingRectangle;

	boundingRectangle.P1 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	boundingRectangle.P2 = XMFLOAT3(0.0f, 1.0f, 0.0f);
	boundingRectangle.P3 = XMFLOAT3(0.0f, 1.0f, 1.0f);
	boundingRectangle.P4 = XMFLOAT3(0.0f, 0.0f, 1.0f);
	yzScalingPlane = new Handle_ScalingPlane(XMLoadFloat3(&xDir), 0.0f, boundingRectangle, /*windowHandle,*/ YZ);
		boundingRectangle.P1 = XMFLOAT3(0.0f, -1.0f, -1.0f);
		boundingRectangle.P2 = XMFLOAT3(0.0f,  0.0f, -1.0f);
		boundingRectangle.P3 = XMFLOAT3(0.0f,  0.0f,  0.0f);
		boundingRectangle.P4 = XMFLOAT3(0.0f, -1.0f,  0.0f);
		yzScalingPlane2 = new Handle_ScalingPlane(-XMLoadFloat3(&xDir), 0.0f, boundingRectangle, /*windowHandle,*/ YZ);

	boundingRectangle.P1 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	boundingRectangle.P2 = XMFLOAT3(0.0f, 0.0f, 1.0f);
	boundingRectangle.P3 = XMFLOAT3(1.0f, 0.0f, 1.0f);
	boundingRectangle.P4 = XMFLOAT3(1.0f, 0.0f, 0.0f);
	zxScalingPlane = new Handle_ScalingPlane(XMLoadFloat3(&yDir), 0.0f, boundingRectangle, /*windowHandle,*/ XZ);
		boundingRectangle.P1 = XMFLOAT3(-1.0f, 0.0f, -1.0f);
		boundingRectangle.P2 = XMFLOAT3(-1.0f, 0.0f,  0.0f);
		boundingRectangle.P3 = XMFLOAT3( 0.0f, 0.0f,  0.0f);
		boundingRectangle.P4 = XMFLOAT3( 0.0f, 0.0f, -1.0f);
		zxScalingPlane2 = new Handle_ScalingPlane(-XMLoadFloat3(&yDir), 0.0f, boundingRectangle, /*windowHandle,*/ XZ);

	boundingRectangle.P1 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	boundingRectangle.P2 = XMFLOAT3(0.0f, 1.0f, 0.0f);
	boundingRectangle.P3 = XMFLOAT3(1.0f, 1.0f, 0.0f);
	boundingRectangle.P4 = XMFLOAT3(1.0f, 0.0f, 0.0f);
	xyScalingPlane = new Handle_ScalingPlane(XMLoadFloat3(&zDir), 0.0f, boundingRectangle, /*windowHandle,*/ XY);
		boundingRectangle.P1 = XMFLOAT3(-1.0f, -1.0f, 0.0f);
		boundingRectangle.P2 = XMFLOAT3(-1.0f,  0.0f, 0.0f);
		boundingRectangle.P3 = XMFLOAT3( 0.0f,  0.0f, 0.0f);
		boundingRectangle.P4 = XMFLOAT3( 0.0f, -1.0f, 0.0f);
		xyScalingPlane2 = new Handle_ScalingPlane(-XMLoadFloat3(&zDir), 0.0f, boundingRectangle, /*windowHandle,*/ XY);

	//boundingRectangle.P1 = XMFLOAT3(-0.25f, -0.25f, 0.0f);
	//boundingRectangle.P2 = XMFLOAT3(-0.25f,  0.25f, 0.0f);
	//boundingRectangle.P3 = XMFLOAT3( 0.25f,  0.25f, 0.0f);
	//boundingRectangle.P4 = XMFLOAT3( 0.25f, -0.25f, 0.0f);
	//camViewScalingPlane = new Handle_ScalingPlane(XMLoadFloat3(&zDirNeg), 0.0f, boundingRectangle, /*windowHandle,*/ YZ);

	//relateToActiveObjectWorld = false;

	shouldFlipMouseCursor = false;
	xyScalingPlane->setShouldFlipMouseCursor(shouldFlipMouseCursor);
	zxScalingPlane->setShouldFlipMouseCursor(shouldFlipMouseCursor);
	yzScalingPlane->setShouldFlipMouseCursor(shouldFlipMouseCursor);

	scale = 1.0f;
	transformInEntityLocalSpace = true;
}

Tool_Scaling::~Tool_Scaling()
{
	delete xyScalingPlane;
	delete yzScalingPlane;
	delete zxScalingPlane;
	delete xyScalingPlane2;
	delete yzScalingPlane2;
	delete zxScalingPlane2;
	delete omniScalingAxisHandle;

	delete xScalingAxisHandle;
	delete yScalingAxisHandle;
	delete zScalingAxisHandle;
	delete xScalingAxisHandle2;
	delete yScalingAxisHandle2;
	delete zScalingAxisHandle2;

	ReleaseCOM(m_pixelShader);
	ReleaseCOM(m_vertexShader);
	ReleaseCOM(m_ColorSchemeIdBuffer);
	ReleaseCOM(m_WVPBuffer);
	ReleaseCOM(m_inputLayout);

	ReleaseCOM(mMeshTransTool_xAxisBox_VB);
	ReleaseCOM(mMeshTransTool_yAxisBox_VB);
	ReleaseCOM(mMeshTransTool_zAxisBox_VB);
	ReleaseCOM(mMeshTransTool_xAxisBox2_VB);
	ReleaseCOM(mMeshTransTool_yAxisBox2_VB);
	ReleaseCOM(mMeshTransTool_zAxisBox2_VB);
	ReleaseCOM(mMeshTransTool_omniAxisBox_VB);
	ReleaseCOM(mMeshTransTool_axisBox_IB);

	ReleaseCOM(mMeshTransTool_yzPlane_VB);
	ReleaseCOM(mMeshTransTool_zxPlane_VB);
	ReleaseCOM(mMeshTransTool_xyPlane_VB);
	ReleaseCOM(mMeshTransTool_yzPlane2_VB);
	ReleaseCOM(mMeshTransTool_zxPlane2_VB);
	ReleaseCOM(mMeshTransTool_xyPlane2_VB);
	ReleaseCOM(mMeshTransTool_viewPlane_VB);

	//ReleaseCOM(mMeshTransTool_yzTriangleListRectangle_VB);
	//ReleaseCOM(mMeshTransTool_zxTriangleListRectangle_VB);
	//ReleaseCOM(mMeshTransTool_xyTriangleListRectangle_VB);
	//ReleaseCOM(mMeshTransTool_viewPlaneTriangleListRectangle_VB);
}

void Tool_Scaling::setIsVisible(bool &isVisible)
{
	this->isVisible = isVisible;
}

/* Called for an instance of picking, possibly resulting in the tool being selected. */
bool Tool_Scaling::tryForSelection(MyRectangle &selectionRectangle, XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, XMMATRIX &camProj, POINT &mouseCursorPoint)
{
	bool boxSelected = false;
	bool aScalingToolHandleWasSelected = false;

	bool rayIntersectsWithToolBoundingBox = false;
	// Check if the picking ray intersects with any of the translation tool's bounding box.
	// ...
	// TEST: Pretend we hit it.
	rayIntersectsWithToolBoundingBox = true;
	
	if(activeEntity.isValid()) // Necessary add for the multi-trans functionality. Previously, updateWorld was repeatedly call during hack-y resettings of the active object, which is now only set once, with selection events.
		updateWorld();

	if(rayIntersectsWithToolBoundingBox)
	{
		// Check if the ray intersects with any of the control handles.
		
		float distanceToPointOfIntersection;
		
		float distanceToClosestPointOfIntersection = FLT_MAX;

		boxSelected = xScalingAxisHandle->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, distanceToPointOfIntersection);
		if(boxSelected)
		{
			distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
			currentlySelectedAxis = xScalingAxisHandle;

			aScalingToolHandleWasSelected = true;
		}

			boxSelected = xScalingAxisHandle2->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, distanceToPointOfIntersection);
			if(boxSelected)
			{
				if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
				{
					distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
					currentlySelectedAxis = xScalingAxisHandle2;

					aScalingToolHandleWasSelected = true;
				}
			}

		boxSelected = yScalingAxisHandle->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, distanceToPointOfIntersection);
		if(boxSelected)
		{
			if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
			{
				distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
				currentlySelectedAxis = yScalingAxisHandle;

				aScalingToolHandleWasSelected = true;
			}
		}

			boxSelected = yScalingAxisHandle2->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, distanceToPointOfIntersection);
			if(boxSelected)
			{
				if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
				{
					distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
					currentlySelectedAxis = yScalingAxisHandle2;

					aScalingToolHandleWasSelected = true;
				}
			}

		boxSelected = zScalingAxisHandle->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, distanceToPointOfIntersection);
		if(boxSelected)
		{
			if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
			{
				distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
				currentlySelectedAxis = zScalingAxisHandle;

				aScalingToolHandleWasSelected = true;
			}
		}

			boxSelected = zScalingAxisHandle2->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, distanceToPointOfIntersection);
			if(boxSelected)
			{
				if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
				{
					distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
					currentlySelectedAxis = zScalingAxisHandle2;

					aScalingToolHandleWasSelected = true;
				}
			}

		if(!currentlySelectedAxis)
		{
			bool planeSelected = omniScalingAxisHandle->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, distanceToPointOfIntersection, mouseCursorPoint);

			// If the camera view translation plane is intersected, it is always selected...
			if(planeSelected)
			{
				currentlySelectedHandle = omniScalingAxisHandle;
				aScalingToolHandleWasSelected = true;
			}
		}

		if(!currentlySelectedPlane && !currentlySelectedAxis && !currentlySelectedHandle) // ... Else, the others are tested for intersection and prioritized by intersection point distance:
		{
			bool planeSelected = false;

			float distanceToClosestPointOfIntersection = FLT_MAX;

			planeSelected = xyScalingPlane->tryForSelection(rayOrigin, rayDir, camView, distanceToPointOfIntersection);
			if(planeSelected)
			{
				distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
				currentlySelectedPlane = xyScalingPlane;
				aScalingToolHandleWasSelected = true;
			}

				planeSelected = xyScalingPlane2->tryForSelection(rayOrigin, rayDir, camView, distanceToPointOfIntersection);
				if(planeSelected)
				{
					if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
					{
						distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
						currentlySelectedPlane = xyScalingPlane2;
						aScalingToolHandleWasSelected = true;
					}
				}

			planeSelected = yzScalingPlane->tryForSelection(rayOrigin, rayDir, camView, distanceToPointOfIntersection);
			if(planeSelected)
			{
				if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
				{
					distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
					currentlySelectedPlane = yzScalingPlane;
					aScalingToolHandleWasSelected = true;
				}
			}

				planeSelected = yzScalingPlane2->tryForSelection(rayOrigin, rayDir, camView, distanceToPointOfIntersection);
				if(planeSelected)
				{
					if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
					{
						distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
						currentlySelectedPlane = yzScalingPlane2;
						aScalingToolHandleWasSelected = true;
					}
				}

			planeSelected = zxScalingPlane->tryForSelection(rayOrigin, rayDir, camView, distanceToPointOfIntersection);
			if(planeSelected)
			{
				if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
				{
					distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
					currentlySelectedPlane = zxScalingPlane;
					aScalingToolHandleWasSelected = true;
				}
			}

				planeSelected = zxScalingPlane2->tryForSelection(rayOrigin, rayDir, camView, distanceToPointOfIntersection);
				if(planeSelected)
				{
					if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
					{
						distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
						currentlySelectedPlane = zxScalingPlane2;
						aScalingToolHandleWasSelected = true;
					}
				}
		}
	}
	
	isSelected = aScalingToolHandleWasSelected;

	if(aScalingToolHandleWasSelected)
	{
		// Set the cursor icon to the one indicating that the free rotation sphere is being handled.
		SEND_EVENT(&Event_SetCursor(Event_SetCursor::CursorShape::SceneCursor_Pointer));
	}

	return aScalingToolHandleWasSelected;
}

void Tool_Scaling::tryForHover(MyRectangle &selectionRectangle, XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView)
{
}

/* Called to bind the translatable object to the tool, so its translation can be modified. */
void Tool_Scaling::setActiveObject(int entityId)
{
	DataMapper<Data::Selected> map_selected;
	Entity* e;

	originalScalesOfSelectedEntities.clear();

	bool thereIsAtLeastOneSelectedEntity = map_selected.hasNext();
	while(map_selected.hasNext())
	{
		e = map_selected.nextEntity();
		//Data::Selected* d_selected = e->fetchData<Data::Selected>();

		XMVECTOR scale = e->fetchData<Data::Transform>()->scale; //XMMATRIX world = e->fetchData<Data::Transform>()->toWorldMatrix();
		XMFLOAT3 origScale;
		XMStoreFloat3(&origScale, scale);
		originalScalesOfSelectedEntities.push_back(origScale);
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

	xyScalingPlane->resetScalingDelta();
	zxScalingPlane->resetScalingDelta();
	yzScalingPlane->resetScalingDelta();

	//----

	//this->activeEntityId = entityId;

	//// Set the visual and bounding components of the translation tool to the pivot point of the active object.
	//updateWorld();

	//XMMATRIX world = activeEntity->fetchData<Data::Transform>()->toWorldMatrix();

	//xyScalingPlane->resetScalingDelta();
	//zxScalingPlane->resetScalingDelta();
	//yzScalingPlane->resetScalingDelta();

	//XMStoreFloat4x4(&originalWorldOfActiveObject, world);
}

void Tool_Scaling::updateWorld()
{
	//if(!relateToActiveObjectWorld)
	//{
		// Just get the position of the active object, but keep the default orientation.
		Matrix newWorld = XMMatrixIdentity();

		Data::Transform* trans = activeEntity->fetchData<Data::Transform>();

		newWorld._41 = trans->position.x; //objectWorld._41;
		newWorld._42 = trans->position.y; //objectWorld._42;
		newWorld._43 = trans->position.z; //objectWorld._43;

		// Update the translation tool's (distance-from-camera-adjusted) scale.
		//newWorld._11 = scale;
		//newWorld._22 = scale;
		//newWorld._33 = scale;
		XMStoreFloat4x4(&world, newWorld);

		XMMATRIX logicalWorld = XMLoadFloat4x4(&getWorld_logical());

		// Transform all the controls.
		xScalingAxisHandle->setWorld(logicalWorld);
		yScalingAxisHandle->setWorld(logicalWorld);
		zScalingAxisHandle->setWorld(logicalWorld);
			xScalingAxisHandle2->setWorld(logicalWorld);
			yScalingAxisHandle2->setWorld(logicalWorld);
			zScalingAxisHandle2->setWorld(logicalWorld);

		yzScalingPlane->setWorld(logicalWorld);
		zxScalingPlane->setWorld(logicalWorld);
		xyScalingPlane->setWorld(logicalWorld);
			yzScalingPlane2->setWorld(logicalWorld);
			zxScalingPlane2->setWorld(logicalWorld);
			xyScalingPlane2->setWorld(logicalWorld);

		omniScalingAxisHandle->setWorld(logicalWorld);

		//camViewScalingPlane->setWorld(XMLoadFloat4x4(&getWorld_viewPlaneTranslationControl_logical()));
	//}
	//else
	//{
	//	// Get the position and orientation of the active object.
	//	world = Entity(activeEntityId).fetchData<Data::Transform>()->toWorldMatrix();

	//	// Update the translation tool's (distance-from-camera-adjusted) scale.
	//	world._11 = scale;
	//	world._22 = scale;
	//	world._33 = scale;
	//	//XMStoreFloat4x4(&world, newWorld);

	//	XMMATRIX logicalWorld = XMLoadFloat4x4(&getWorld_logical());

	//	// Transform all the controls.
	//	xScalingAxisHandle->setWorld(logicalWorld);
	//	yScalingAxisHandle->setWorld(logicalWorld);
	//	zScalingAxisHandle->setWorld(logicalWorld);
	//	xScalingAxisHandle2->setWorld(logicalWorld);
	//	yScalingAxisHandle2->setWorld(logicalWorld);
	//	zScalingAxisHandle2->setWorld(logicalWorld);

	//	yzScalingPlane->setWorld(logicalWorld);
	//	zxScalingPlane->setWorld(logicalWorld);
	//	xyScalingPlane->setWorld(logicalWorld);
	//}
}

/* Transform all controls to the local coord. sys. of the active object. */
void Tool_Scaling::setRelateToActiveObjectWorld(bool relateToActiveObjectWorld)
{
	this->relateToActiveObjectWorld = relateToActiveObjectWorld;

	if(activeEntity.isValid())
		updateWorld();
}

/* Called to see if the translation tool is (still) active. */
bool Tool_Scaling::getIsSelected()
{
	return isSelected;
}

/* Called to set the entity at whose pivot the tool is to be displayed, when a selection of one or more entities has been made. */
void Tool_Scaling::setEntityAtWhosePivotTheToolIsToBeDisplayed(int entityId)
{
}

/* Called to send updated parameters to the translation tool, if it is still active. */
void Tool_Scaling::update(MyRectangle &selectionRectangle, XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, XMMATRIX &camProj, D3D11_VIEWPORT &theViewport, POINT &mouseCursorPoint)
{
	XMVECTOR scaleDelta = XMVectorSet(0,0,0,0);
	if(currentlySelectedAxis)
	{
		currentlySelectedAxis->update(rayOrigin, rayDir, camView);
		currentlySelectedAxis->pickAxisPlane(rayOrigin, rayDir, camView, camProj);
		currentlySelectedAxis->calcLastScalingDelta();
		scaleDelta = currentlySelectedAxis->getTotalScalingDelta();

		if(currentlySelectedAxis == xScalingAxisHandle2 || currentlySelectedAxis == yScalingAxisHandle2 || currentlySelectedAxis == zScalingAxisHandle2)
		{
			scaleDelta = -scaleDelta;
		}
		
		float toolScaleDependantScaleFactor = scale;
		int i = 0;
		Entity* e;
		DataMapper<Data::Selected> map_selected;
		while(map_selected.hasNext())
		{
			e = map_selected.nextEntity();

			float scaleDependantScaleFactorX = originalScalesOfSelectedEntities.at(i).x * 0.1f;
			float scaleDependantScaleFactorY = originalScalesOfSelectedEntities.at(i).y * 0.1f;
			float scaleDependantScaleFactorZ = originalScalesOfSelectedEntities.at(i).z * 0.1f;
			if(scaleDependantScaleFactorX < 0.1f)
				scaleDependantScaleFactorX = 0.1f;
			if(scaleDependantScaleFactorY < 0.1f)
				scaleDependantScaleFactorY = 0.1f;
			if(scaleDependantScaleFactorZ < 0.1f)
				scaleDependantScaleFactorZ = 0.1f;

			XMVECTOR scaleValues;
			scaleValues.m128_f32[0] = scaleDelta.m128_f32[0] * toolScaleDependantScaleFactor * scaleDependantScaleFactorX;
			scaleValues.m128_f32[1] = scaleDelta.m128_f32[1] * toolScaleDependantScaleFactor * scaleDependantScaleFactorY;
			scaleValues.m128_f32[2] = scaleDelta.m128_f32[2] * toolScaleDependantScaleFactor * scaleDependantScaleFactorZ;

			e->fetchData<Data::Transform>()->scale = Vector3(XMLoadFloat3(&originalScalesOfSelectedEntities.at(i))) + Vector3(scaleValues);

			if(e->fetchData<Data::Transform>()->scale.x < 0.01f)
				e->fetchData<Data::Transform>()->scale.x = 0.01f;
			if(e->fetchData<Data::Transform>()->scale.y < 0.01f)
				e->fetchData<Data::Transform>()->scale.y = 0.01f;
			if(e->fetchData<Data::Transform>()->scale.z < 0.01f)
				e->fetchData<Data::Transform>()->scale.z = 0.01f;

			++i;
		}
	}
	else if(currentlySelectedPlane)
	{
		currentlySelectedPlane->pickPlane(rayOrigin, rayDir, camView, camProj, theViewport);
		currentlySelectedPlane->calcLastScalingDelta();
		scaleDelta = currentlySelectedPlane->getTotalScalingDelta();

		if(currentlySelectedPlane == xyScalingPlane2 || currentlySelectedPlane == yzScalingPlane2 || currentlySelectedPlane == zxScalingPlane2)
		{
			scaleDelta = -scaleDelta;
		}

		float toolScaleDependantScaleFactor = scale;
		int i = 0;
		Entity* e;
		DataMapper<Data::Selected> map_selected;
		while(map_selected.hasNext())
		{
			e = map_selected.nextEntity();

			float scaleDependantScaleFactorX = originalScalesOfSelectedEntities.at(i).x * 0.1f;
			float scaleDependantScaleFactorY = originalScalesOfSelectedEntities.at(i).y * 0.1f;
			float scaleDependantScaleFactorZ = originalScalesOfSelectedEntities.at(i).z * 0.1f;
			if(scaleDependantScaleFactorX < 0.1f)
				scaleDependantScaleFactorX = 0.1f;
			if(scaleDependantScaleFactorY < 0.1f)
				scaleDependantScaleFactorY = 0.1f;
			if(scaleDependantScaleFactorZ < 0.1f)
				scaleDependantScaleFactorZ = 0.1f;

			XMVECTOR scaleValues;
			scaleValues.m128_f32[0] = scaleDelta.m128_f32[0] * toolScaleDependantScaleFactor * scaleDependantScaleFactorX;
			scaleValues.m128_f32[1] = scaleDelta.m128_f32[1] * toolScaleDependantScaleFactor * scaleDependantScaleFactorY;
			scaleValues.m128_f32[2] = scaleDelta.m128_f32[2] * toolScaleDependantScaleFactor * scaleDependantScaleFactorZ;

			e->fetchData<Data::Transform>()->scale = Vector3(XMLoadFloat3(&originalScalesOfSelectedEntities.at(i))) + Vector3(scaleValues);

			if(e->fetchData<Data::Transform>()->scale.x < 0.01f)
				e->fetchData<Data::Transform>()->scale.x = 0.01f;
			if(e->fetchData<Data::Transform>()->scale.y < 0.01f)
				e->fetchData<Data::Transform>()->scale.y = 0.01f;
			if(e->fetchData<Data::Transform>()->scale.z < 0.01f)
				e->fetchData<Data::Transform>()->scale.z = 0.01f;

			++i;
		}
	}
	else if(currentlySelectedHandle)
	{
		omniScalingAxisHandle->update(mouseCursorPoint);
		POINT totalCursorDeltas = omniScalingAxisHandle->getTotalScalingDelta();

		float xScaleFactor = 0.010f;
		float yScaleFactor = 0.005f;
		
		float xScaleContribution = totalCursorDeltas.x * xScaleFactor;
		float yScaleContribution = totalCursorDeltas.y * yScaleFactor;

		scaleDelta = XMVectorSet(xScaleContribution + yScaleContribution, xScaleContribution + yScaleContribution, xScaleContribution + yScaleContribution, 1.0f);

		XMVECTOR scalePercentages = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f) + scaleDelta;

		float toolScaleDependantScaleFactor = scale;
		int i = 0;
		Entity* e;
		DataMapper<Data::Selected> map_selected;
		while(map_selected.hasNext())
		{
			e = map_selected.nextEntity();

			float scaleDependantScaleFactorX = originalScalesOfSelectedEntities.at(i).x * 0.1f;
			float scaleDependantScaleFactorY = originalScalesOfSelectedEntities.at(i).y * 0.1f;
			float scaleDependantScaleFactorZ = originalScalesOfSelectedEntities.at(i).z * 0.1f;
			if(scaleDependantScaleFactorX < 0.1f)
				scaleDependantScaleFactorX = 0.1f;
			if(scaleDependantScaleFactorY < 0.1f)
				scaleDependantScaleFactorY = 0.1f;
			if(scaleDependantScaleFactorZ < 0.1f)
				scaleDependantScaleFactorZ = 0.1f;

			float scaleDependantScaleFactorAvg = (scaleDependantScaleFactorX + scaleDependantScaleFactorY + scaleDependantScaleFactorZ) / 3;

			scalePercentages.m128_f32[0] = scalePercentages.m128_f32[0]; // * scaleDependantScaleFactorAvg * toolScaleDependantScaleFactor;
			scalePercentages.m128_f32[1] = scalePercentages.m128_f32[1]; // * scaleDependantScaleFactorAvg * toolScaleDependantScaleFactor;
			scalePercentages.m128_f32[2] = scalePercentages.m128_f32[2]; // * scaleDependantScaleFactorAvg * toolScaleDependantScaleFactor;

			e->fetchData<Data::Transform>()->scale = Vector3(XMLoadFloat3(&originalScalesOfSelectedEntities.at(i))) * Vector3(scalePercentages);

			if(e->fetchData<Data::Transform>()->scale.x < 0.01f)
				e->fetchData<Data::Transform>()->scale.x = 0.01f;
			if(e->fetchData<Data::Transform>()->scale.y < 0.01f)
				e->fetchData<Data::Transform>()->scale.y = 0.01f;
			if(e->fetchData<Data::Transform>()->scale.z < 0.01f)
				e->fetchData<Data::Transform>()->scale.z = 0.01f;

			++i;
		}
	}

	Event_SelectedEntitiesHaveBeenTransformed transformEvent;
	SEND_EVENT(&transformEvent);
}

/* Called when the translation tool is unselected, which makes any hitherto made translation final (and undoable). */
void Tool_Scaling::unselect()
{
	// Set the controls' visual and bounding components to the active object's new position and orientation.
	//updateWorld();

	if(currentlySelectedPlane)
	{
		currentlySelectedPlane->unselect();
		currentlySelectedPlane = NULL;
	}

	if(currentlySelectedAxis)
	{
		currentlySelectedAxis->unselect();
		currentlySelectedAxis = NULL;
	}
	
	if(currentlySelectedHandle)
	{
		currentlySelectedHandle->unselect();
		currentlySelectedHandle = NULL;
	}

	std::vector<Command*> translationCommands;
	DataMapper<Data::Selected> map_selected;
	Entity* e;
	unsigned int i = 0;
	while(map_selected.hasNext())
	{
		e = map_selected.nextEntity();

		Data::Transform* trans = e->fetchData<Data::Transform>();
		Command_ScaleSceneEntity *command = new Command_ScaleSceneEntity(e->id());
		command->setDoScale(trans->scale.x, trans->scale.y, trans->scale.z);
		command->setUndoScale(originalScalesOfSelectedEntities.at(i).x, originalScalesOfSelectedEntities.at(i).y, originalScalesOfSelectedEntities.at(i).z);
		translationCommands.push_back(command);
		
		//SEND_EVENT(&Event_AddToCommandHistory(command, false));
		
		++i;
	}

	SEND_EVENT(&Event_AddToCommandHistory(&translationCommands, false));

	setActiveObject(1);

	SEND_EVENT(&Event_SetCursor(Event_SetCursor::CursorShape::SceneCursor));

	isSelected = false;
}

XMFLOAT4X4 Tool_Scaling::getWorld_logical()
{
	XMFLOAT4X4 world_logical;
	if(transformInEntityLocalSpace)
	{
		Data::Transform* transform = activeEntity->fetchData<Data::Transform>();

		XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);
		XMMATRIX translation = XMMatrixTranslation(transform->position.x, transform->position.y, transform->position.z);
		XMMATRIX rotation = XMMatrixRotationQuaternion(XMVectorSet(transform->rotation.x, transform->rotation.y, transform->rotation.z, transform->rotation.w));
		
		XMStoreFloat4x4(&world_logical, scaling * rotation * translation);
	}

	return world_logical;
}

XMFLOAT4X4 Tool_Scaling::getWorld_visual()
{
	XMFLOAT4X4 world_visual;
	if(transformInEntityLocalSpace)
	{
		Data::Transform* transform = activeEntity->fetchData<Data::Transform>();

		XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);
		XMMATRIX translation = XMMatrixTranslation(transform->position.x, transform->position.y, transform->position.z);

		XMVECTOR rotQuat = transform->rotation; //XMVectorSet(transform->rotation.x, transform->rotation.y, transform->rotation.z, transform->rotation.w);
		XMMATRIX rotation = XMMatrixRotationQuaternion(rotQuat);
		
		XMStoreFloat4x4(&world_visual, scaling * rotation * translation);
	}

	//XMVECTOR trans = Entity(activeEntityId).fetchData<Data::Transform>()->position;
	//XMMATRIX translation = XMMatrixTranslationFromVector(trans);

	//XMMATRIX scaling;
	//scaling = XMMatrixScaling(scale, scale, scale);

	//XMFLOAT4X4 world_visual;
	//XMStoreFloat4x4(&world_visual, scaling * translation);

	return world_visual;
}

XMFLOAT4X4 Tool_Scaling::getWorld_visual_objectRelative()
{
	XMVECTOR trans = activeEntity->fetchData<Data::Transform>()->position;
	XMMATRIX translation = XMMatrixTranslationFromVector(trans);

	XMVECTOR scale = activeEntity->fetchData<Data::Transform>()->scale;
	XMMATRIX scaling = XMMatrixTranslationFromVector(trans);

	XMFLOAT4X4 world_visual_objectRelative;
	XMStoreFloat4x4(&world_visual_objectRelative, scaling * translation);

	return world_visual_objectRelative;
}

void Tool_Scaling::setScale(float &scale)
{
	this->scale = scale;
}

float Tool_Scaling::getScale()
{
	return scale;
}

/* Called to see if the visual component of the translation tool should be rendered. */
bool Tool_Scaling::getIsVisible()
{
	return isVisible;
}

void Tool_Scaling::setShouldFlipMouseCursor(bool shouldFlipMouseCursor)
{
	this->shouldFlipMouseCursor = shouldFlipMouseCursor;

	xyScalingPlane->setShouldFlipMouseCursor(shouldFlipMouseCursor);
	zxScalingPlane->setShouldFlipMouseCursor(shouldFlipMouseCursor);
	yzScalingPlane->setShouldFlipMouseCursor(shouldFlipMouseCursor);
}

void Tool_Scaling::updateViewPlaneTranslationControlWorld(XMFLOAT3 &camViewVector, XMFLOAT3 &camUpVector)
{
	XMVECTOR loadedCamViewVector = XMLoadFloat3(&camViewVector);
	XMVECTOR loadedCamUpVector = XMLoadFloat3(&camUpVector);
	XMVECTOR camRightVector = XMVector3Cross(loadedCamUpVector, loadedCamViewVector);

	XMStoreFloat3((XMFLOAT3*)world_viewPlaneTranslationControl_logical.m[0], camRightVector);
	world_viewPlaneTranslationControl_logical.m[0][3] = 0.0f;
	XMStoreFloat3((XMFLOAT3*)world_viewPlaneTranslationControl_logical.m[1], loadedCamUpVector);
	world_viewPlaneTranslationControl_logical.m[1][3] = 0.0f;
	XMStoreFloat3((XMFLOAT3*)world_viewPlaneTranslationControl_logical.m[2], loadedCamViewVector);
	world_viewPlaneTranslationControl_logical.m[2][3] = 0.0f;

	world_viewPlaneTranslationControl_logical._41 = world.m[3][0];
	world_viewPlaneTranslationControl_logical._42 = world.m[3][1];
	world_viewPlaneTranslationControl_logical._43 = world.m[3][2];
	world_viewPlaneTranslationControl_logical._44 = world.m[3][3];

	XMStoreFloat3((XMFLOAT3*)world_viewPlaneTranslationControl_visual.m[0], camRightVector);
	world_viewPlaneTranslationControl_visual.m[0][3] = 0.0f;
	XMStoreFloat3((XMFLOAT3*)world_viewPlaneTranslationControl_visual.m[1], loadedCamUpVector);
	world_viewPlaneTranslationControl_visual.m[1][3] = 0.0f;
	XMStoreFloat3((XMFLOAT3*)world_viewPlaneTranslationControl_visual.m[2], loadedCamViewVector);
	world_viewPlaneTranslationControl_visual.m[2][3] = 0.0f;

	Vector3 activeEntityPos = activeEntity->fetchData<Data::Transform>()->position;
	world_viewPlaneTranslationControl_visual._41 = activeEntityPos.x;
	world_viewPlaneTranslationControl_visual._42 = activeEntityPos.y;
	world_viewPlaneTranslationControl_visual._43 = activeEntityPos.z;
	world_viewPlaneTranslationControl_visual._44 = 1.0f;
}

XMFLOAT4X4 Tool_Scaling::getWorld_viewPlaneTranslationControl_logical()
{
	XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);

	XMMATRIX loadedWorld = XMLoadFloat4x4(&world_viewPlaneTranslationControl_logical);
	XMFLOAT4X4 logicalWorld;
	XMStoreFloat4x4(&logicalWorld, scaling * loadedWorld);

	return logicalWorld;
}

XMFLOAT4X4 Tool_Scaling::getWorld_viewPlaneTranslationControl_visual()
{
	XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);

	XMMATRIX loadedWorld = XMLoadFloat4x4(&world_viewPlaneTranslationControl_visual);
	XMFLOAT4X4 visualWorld;
	XMStoreFloat4x4(&visualWorld, scaling * loadedWorld);

	return visualWorld;
}

EntityPointer Tool_Scaling::getActiveObject()
{
	return activeEntity;
}
	
void Tool_Scaling::init(ID3D11Device *device, ID3D11DeviceContext *deviceContext)
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

	D3D11_BUFFER_DESC ToolPS_PerFrame_BufferDesc;
	ZeroMemory(&ColorSchemeId_Desc, sizeof(ColorSchemeId_Desc)); //memset(&WVP_Desc, 0, sizeof(WVP_Desc));
	ColorSchemeId_Desc.Usage = D3D11_USAGE_DEFAULT;
	ColorSchemeId_Desc.ByteWidth = 16;
	ColorSchemeId_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	HR(md3dDevice->CreateBuffer(&ColorSchemeId_Desc, NULL, &m_ToolPS_PerFrame_Buffer));

	// Create test mesh for visual translation control.
	
	D3D11_BUFFER_DESC vbd;
	D3D11_SUBRESOURCE_DATA vinitData;

	std::vector<Vertex::PosCol> vertices;

	Vertex::PosCol posCol;

	posCol.Col.w = 1.0f;

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

		// Create obscuring surfaces.

		// XY plane front surface.

		// Triangle A...

			posCol.Col.x = 1.0f; posCol.Col.y = 0.5f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f; // Transparent.
		posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.01f; //posCol.Pos.x = 0.009f; posCol.Pos.y = 0.009f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

			posCol.Col.x = 1.0f; posCol.Col.y = 1.0f; posCol.Col.z = 0.0f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 0.0f; posCol.Pos.y = 1.0f; posCol.Pos.z = 0.01f; // posCol.Pos.x = 0.009f; posCol.Pos.y = 0.99f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

			posCol.Col.x = 1.0f; posCol.Col.y = 0.5f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 1.0f; posCol.Pos.y = 1.0f; posCol.Pos.z = 0.01f; // posCol.Pos.x = 0.99f; posCol.Pos.y = 0.99f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		// Triangle B...
			posCol.Col.x = 1.0f; posCol.Col.y = 0.5f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 1.0f; posCol.Pos.y = 1.0f; posCol.Pos.z = 0.01f; // posCol.Pos.x = 0.99f; posCol.Pos.y = 0.99f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

			posCol.Col.x = 1.0f; posCol.Col.y = 0.0f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 1.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.01f; // posCol.Pos.x = 0.99f; posCol.Pos.y = 0.009f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

			posCol.Col.x = 1.0f; posCol.Col.y = 0.5f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.01f; // posCol.Pos.x = 0.009f; posCol.Pos.y = 0.009f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		// XY plane back surface.

		// Triangle A...

			posCol.Col.x = 1.0f; posCol.Col.y = 0.5f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 1.0f; posCol.Pos.y = 1.0f; posCol.Pos.z = -0.01f; // posCol.Pos.x = 0.99f; posCol.Pos.y = 0.99f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

			posCol.Col.x = 1.0f; posCol.Col.y = 1.0f; posCol.Col.z = 0.0f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 0.0f; posCol.Pos.y = 1.0f; posCol.Pos.z = -0.01f; // posCol.Pos.x = 0.009f; posCol.Pos.y = 0.99f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

			posCol.Col.x = 1.0f; posCol.Col.y = 0.5f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = -0.01f; //posCol.Pos.x = 0.009f; posCol.Pos.y = 0.009f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		// Triangle B...
			posCol.Col.x = 1.0f; posCol.Col.y = 0.5f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = -0.01f; // posCol.Pos.x = 0.009f; posCol.Pos.y = 0.009f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

			posCol.Col.x = 1.0f; posCol.Col.y = 0.0f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 1.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = -0.01f; // posCol.Pos.x = 0.99f; posCol.Pos.y = 0.009f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

			posCol.Col.x = 1.0f; posCol.Col.y = 0.5f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 1.0f; posCol.Pos.y = 1.0f; posCol.Pos.z = -0.01f; // posCol.Pos.x = 0.99f; posCol.Pos.y = 0.99f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

			// XY plane 2 front surface.

			// Triangle A...

				posCol.Col.x = 1.0f; posCol.Col.y = 0.5f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
			posCol.Pos.x = -1.0f; posCol.Pos.y = -1.0f; posCol.Pos.z = 0.01f; //posCol.Pos.x = 0.009f; posCol.Pos.y = 0.009f; posCol.Pos.z = 0.0f;
			vertices.push_back(posCol);

				posCol.Col.x = 1.0f; posCol.Col.y = 0.0f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
			posCol.Pos.x = -1.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.01f; // posCol.Pos.x = 0.009f; posCol.Pos.y = 0.99f; posCol.Pos.z = 0.0f;
			vertices.push_back(posCol);

				posCol.Col.x = 1.0f; posCol.Col.y = 0.5f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
			posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.01f; // posCol.Pos.x = 0.99f; posCol.Pos.y = 0.99f; posCol.Pos.z = 0.0f;
			vertices.push_back(posCol);

			// Triangle B...

				posCol.Col.x = 1.0f; posCol.Col.y = 0.5f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
			posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.01f; // posCol.Pos.x = 0.99f; posCol.Pos.y = 0.99f; posCol.Pos.z = 0.0f;
			vertices.push_back(posCol);

				posCol.Col.x = 1.0f; posCol.Col.y = 1.0f; posCol.Col.z = 0.0f; posCol.Col.w = 0.0f;
			posCol.Pos.x = 0.0f; posCol.Pos.y = -1.0f; posCol.Pos.z = 0.01f; // posCol.Pos.x = 0.99f; posCol.Pos.y = 0.009f; posCol.Pos.z = 0.0f;
			vertices.push_back(posCol);

				posCol.Col.x = 1.0f; posCol.Col.y = 0.5f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
			posCol.Pos.x = -1.0f; posCol.Pos.y = -1.0f; posCol.Pos.z = 0.01f; // posCol.Pos.x = 0.009f; posCol.Pos.y = 0.009f; posCol.Pos.z = 0.0f;
			vertices.push_back(posCol);

			// XY plane 2 back surface.

			// Triangle A...

				posCol.Col.x = 1.0f; posCol.Col.y = 0.5f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
			posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = -0.01f; // posCol.Pos.x = 0.99f; posCol.Pos.y = 0.99f; posCol.Pos.z = 0.0f;
			vertices.push_back(posCol);

				posCol.Col.x = 1.0f; posCol.Col.y = 0.0f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
			posCol.Pos.x = -1.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = -0.01f; // posCol.Pos.x = 0.009f; posCol.Pos.y = 0.99f; posCol.Pos.z = 0.0f;
			vertices.push_back(posCol);

				posCol.Col.x = 1.0f; posCol.Col.y = 0.5f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
			posCol.Pos.x = -1.0f; posCol.Pos.y = -1.0f; posCol.Pos.z = -0.01f; //posCol.Pos.x = 0.009f; posCol.Pos.y = 0.009f; posCol.Pos.z = 0.0f;
			vertices.push_back(posCol);

			// Triangle B...

				posCol.Col.x = 1.0f; posCol.Col.y = 0.5f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
			posCol.Pos.x = -1.0f; posCol.Pos.y = -1.0f; posCol.Pos.z = -0.01f; // posCol.Pos.x = 0.009f; posCol.Pos.y = 0.009f; posCol.Pos.z = 0.0f;
			vertices.push_back(posCol);

				posCol.Col.x = 1.0f; posCol.Col.y = 1.0f; posCol.Col.z = 0.0f; posCol.Col.w = 0.0f;
			posCol.Pos.x = 0.0f; posCol.Pos.y = -1.0f; posCol.Pos.z = -0.01f; // posCol.Pos.x = 0.99f; posCol.Pos.y = 0.009f; posCol.Pos.z = 0.0f;
			vertices.push_back(posCol);

				posCol.Col.x = 1.0f; posCol.Col.y = 0.5f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
			posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = -0.01f; // posCol.Pos.x = 0.99f; posCol.Pos.y = 0.99f; posCol.Pos.z = 0.0f;
			vertices.push_back(posCol);

		// YZ plane front surface.

		// Triangle A...

			posCol.Col.x = 0.5f; posCol.Col.y = 1.0f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
		posCol.Pos.x = -0.01f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

			posCol.Col.x = 1.0f; posCol.Col.y = 1.0f; posCol.Col.z = 0.0f; posCol.Col.w = 0.0f;
		posCol.Pos.x = -0.01f; posCol.Pos.y = 1.0f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

			posCol.Col.x = 0.5f; posCol.Col.y = 1.0f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
		posCol.Pos.x = -0.01f; posCol.Pos.y = 1.0f; posCol.Pos.z = 1.0f;
		vertices.push_back(posCol);

		// Triangle B...

			posCol.Col.x = 0.5f; posCol.Col.y = 1.0f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
		posCol.Pos.x = -0.01f; posCol.Pos.y = 1.0f; posCol.Pos.z = 1.0f;
		vertices.push_back(posCol);

			posCol.Col.x = 0.0f; posCol.Col.y = 1.0f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
		posCol.Pos.x = -0.01f; posCol.Pos.y = 0.0f; posCol.Pos.z = 1.0f;
		vertices.push_back(posCol);

			posCol.Col.x = 0.5f; posCol.Col.y = 1.0f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
		posCol.Pos.x = -0.01f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		// YZ plane back surface.

		// Triangle A...

			posCol.Col.x = 0.5f; posCol.Col.y = 1.0f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 0.01f; posCol.Pos.y = 1.0f; posCol.Pos.z = 1.0f;
		vertices.push_back(posCol);

			posCol.Col.x = 1.0f; posCol.Col.y = 1.0f; posCol.Col.z = 0.0f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 0.01f; posCol.Pos.y = 1.0f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

			posCol.Col.x = 0.5f; posCol.Col.y = 1.0f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 0.01f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		// Triangle B...

			posCol.Col.x = 0.5f; posCol.Col.y = 1.0f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 0.01f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f; 
		vertices.push_back(posCol);

			posCol.Col.x = 0.0f; posCol.Col.y = 1.0f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 0.01f; posCol.Pos.y = 0.0f; posCol.Pos.z = 1.0f;
		vertices.push_back(posCol);

			posCol.Col.x = 0.5f; posCol.Col.y = 1.0f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 0.01f; posCol.Pos.y = 1.0f; posCol.Pos.z = 1.0f;
		vertices.push_back(posCol);

			// YZ plane 2 front surface.

			// Triangle A...

				posCol.Col.x = 0.5f; posCol.Col.y = 1.0f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
			posCol.Pos.x = -0.01f; posCol.Pos.y = -1.0f; posCol.Pos.z = -1.0f;
			vertices.push_back(posCol);

				posCol.Col.x = 0.0f; posCol.Col.y = 1.0f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
			posCol.Pos.x = -0.01f; posCol.Pos.y = 0.0f; posCol.Pos.z = -1.0f;
			vertices.push_back(posCol);

				posCol.Col.x = 0.5f; posCol.Col.y = 1.0f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
			posCol.Pos.x = -0.01f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
			vertices.push_back(posCol);

			// Triangle B...

				posCol.Col.x = 0.5f; posCol.Col.y = 1.0f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
			posCol.Pos.x = -0.01f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
			vertices.push_back(posCol);

				posCol.Col.x = 1.0f; posCol.Col.y = 1.0f; posCol.Col.z = 0.0f; posCol.Col.w = 0.0f;
			posCol.Pos.x = -0.01f; posCol.Pos.y = -1.0f; posCol.Pos.z = 0.0f;
			vertices.push_back(posCol);

				posCol.Col.x = 0.5f; posCol.Col.y = 1.0f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
			posCol.Pos.x = -0.01f; posCol.Pos.y = -1.0f; posCol.Pos.z = -1.0f;
			vertices.push_back(posCol);

			// YZ plane 2 back surface.

			// Triangle A...

				posCol.Col.x = 0.5f; posCol.Col.y = 1.0f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
			posCol.Pos.x = 0.01f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
			vertices.push_back(posCol);

				posCol.Col.x = 0.0f; posCol.Col.y = 1.0f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
			posCol.Pos.x = 0.01f; posCol.Pos.y = 0.0f; posCol.Pos.z = -1.0f;
			vertices.push_back(posCol);

				posCol.Col.x = 0.5f; posCol.Col.y = 1.0f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
			posCol.Pos.x = 0.01f; posCol.Pos.y = -1.0f; posCol.Pos.z = -1.0f;
			vertices.push_back(posCol);

			// Triangle B...

				posCol.Col.x = 0.5f; posCol.Col.y = 1.0f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
			posCol.Pos.x = 0.01f; posCol.Pos.y = -1.0f; posCol.Pos.z = -1.0f;
			vertices.push_back(posCol);
				
				posCol.Col.x = 1.0f; posCol.Col.y = 1.0f; posCol.Col.z = 0.0f; posCol.Col.w = 0.0f;
			posCol.Pos.x = 0.01f; posCol.Pos.y = -1.0f; posCol.Pos.z = 0.0f;
			vertices.push_back(posCol);

				posCol.Col.x = 0.5f; posCol.Col.y = 1.0f; posCol.Col.z = 0.5f; posCol.Col.w = 0.0f;
			posCol.Pos.x = 0.01f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
			vertices.push_back(posCol);

		// ZX plane front surface.

		// Triangle A...

			posCol.Col.x = 0.5f; posCol.Col.y = 0.5f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 0.0f; posCol.Pos.y = -0.01f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

			posCol.Col.x = 0.0f; posCol.Col.y = 1.0f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 0.0f; posCol.Pos.y = -0.01f; posCol.Pos.z = 1.0f;
		vertices.push_back(posCol);

			posCol.Col.x = 0.5f; posCol.Col.y = 0.5f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 1.0f; posCol.Pos.y = -0.01f; posCol.Pos.z = 1.0f;
		vertices.push_back(posCol);

		// Triangle B...

			posCol.Col.x = 0.5f; posCol.Col.y = 0.5f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 1.0f; posCol.Pos.y = -0.01f; posCol.Pos.z = 1.0f;
		vertices.push_back(posCol);

			posCol.Col.x = 1.0f; posCol.Col.y = 0.0f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 1.0f; posCol.Pos.y = -0.01f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

			posCol.Col.x = 0.5f; posCol.Col.y = 0.5f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 0.0f; posCol.Pos.y = -0.01f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		// ZX plane back surface.

		// Triangle A...
			posCol.Col.x = 0.5f; posCol.Col.y = 0.5f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 1.0f; posCol.Pos.y = 0.01f; posCol.Pos.z = 1.0f;
		vertices.push_back(posCol);

			posCol.Col.x = 0.0f; posCol.Col.y = 1.0f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 0.0f; posCol.Pos.y = 0.01f; posCol.Pos.z = 1.0f;
		vertices.push_back(posCol);
		
			posCol.Col.x = 0.5f; posCol.Col.y = 0.5f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 0.0f; posCol.Pos.y = 0.01f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		// Triangle B...
			posCol.Col.x = 0.5f; posCol.Col.y = 0.5f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 0.0f; posCol.Pos.y = 0.01f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);
		
			posCol.Col.x = 1.0f; posCol.Col.y = 0.0f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 1.0f; posCol.Pos.y = 0.01f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);
		
			posCol.Col.x = 0.5f; posCol.Col.y = 0.5f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
		posCol.Pos.x = 1.0f; posCol.Pos.y = 0.01f; posCol.Pos.z = 1.0f;
		vertices.push_back(posCol);

			// ZX plane 2 front surface.

			// Triangle A...

			// Triangle A...

				posCol.Col.x = 0.5f; posCol.Col.y = 0.5f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
			posCol.Pos.x = -1.0f; posCol.Pos.y = -0.01f; posCol.Pos.z = -1.0f;
			vertices.push_back(posCol);

				posCol.Col.x = 1.0f; posCol.Col.y = 0.0f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
			posCol.Pos.x = -1.0f; posCol.Pos.y = -0.01f; posCol.Pos.z = 0.0f;
			vertices.push_back(posCol);

				posCol.Col.x = 0.5f; posCol.Col.y = 0.5f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
			posCol.Pos.x = 0.0f; posCol.Pos.y = -0.01f; posCol.Pos.z = 0.0f;
			vertices.push_back(posCol);

			// Triangle B...

				posCol.Col.x = 0.5f; posCol.Col.y = 0.5f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
			posCol.Pos.x = 0.0f; posCol.Pos.y = -0.01f; posCol.Pos.z = 0.0f;
			vertices.push_back(posCol);

				posCol.Col.x = 0.0f; posCol.Col.y = 1.0f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
			posCol.Pos.x = 0.0f; posCol.Pos.y = -0.01f; posCol.Pos.z = -1.0f;
			vertices.push_back(posCol);

				posCol.Col.x = 0.5f; posCol.Col.y = 0.5f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
			posCol.Pos.x = -1.0f; posCol.Pos.y = -0.01f; posCol.Pos.z = -1.0f;
			vertices.push_back(posCol);

			// ZY plane 2 back surface.

			// Triangle A...

				posCol.Col.x = 0.5f; posCol.Col.y = 0.5f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
			posCol.Pos.x = 0.0f; posCol.Pos.y = 0.01f; posCol.Pos.z = 0.0f;
			vertices.push_back(posCol);

				posCol.Col.x = 1.0f; posCol.Col.y = 0.0f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
			posCol.Pos.x = -1.0f; posCol.Pos.y = 0.01f; posCol.Pos.z = 0.0f;
			vertices.push_back(posCol);

				posCol.Col.x = 0.5f; posCol.Col.y = 0.5f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
			posCol.Pos.x = -1.0f; posCol.Pos.y = 0.01f; posCol.Pos.z = -1.0f;
			vertices.push_back(posCol);

			// Triangle B...
				posCol.Col.x = 0.5f; posCol.Col.y = 0.5f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
			posCol.Pos.x = -1.0f; posCol.Pos.y = 0.01f; posCol.Pos.z = -1.0f;
			vertices.push_back(posCol);

				posCol.Col.x = 0.0f; posCol.Col.y = 1.0f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
			posCol.Pos.x = 0.0f; posCol.Pos.y = 0.01f; posCol.Pos.z = -1.0f;
			vertices.push_back(posCol);

				posCol.Col.x = 0.5f; posCol.Col.y = 0.5f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
			posCol.Pos.x = 0.0f; posCol.Pos.y = 0.01f; posCol.Pos.z = 0.0f;
			vertices.push_back(posCol);

		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex::PosCol) * 72;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vinitData.pSysMem = &vertices[0];
		HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransTool_obscuringRectangles_VB));

		vertices.clear();

	// View triangle-list rectangle.

		posCol.Col.x = 1.0f; posCol.Col.y = 1.0f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
	posCol.Pos.x = -0.25f; posCol.Pos.y = -0.25f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);

		posCol.Col.x = 0.0f; posCol.Col.y = 0.0f; posCol.Col.z = 0.0f; posCol.Col.w = 0.0f;
	posCol.Pos.x = -0.25f; posCol.Pos.y =  0.25f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);

		posCol.Col.x = 1.0f; posCol.Col.y = 1.0f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
	posCol.Pos.x =  0.25f; posCol.Pos.y =  0.25f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);
	
		posCol.Col.x = 0.0f; posCol.Col.y = 0.0f; posCol.Col.z = 0.0f; posCol.Col.w = 0.0f;
	posCol.Pos.x =  0.25f; posCol.Pos.y = -0.25f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);

		posCol.Col.x = 1.0f; posCol.Col.y = 1.0f; posCol.Col.z = 1.0f; posCol.Col.w = 0.0f;
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
	
	XMFLOAT3 xDir(1.0f, 0.0f, 0.0f);
	XMFLOAT3 yDir(0.0f, 1.0f, 0.0f);
	XMFLOAT3 zDir(0.0f, 0.0f, 1.0f);

	// X boxes.

	GeometryGenerator::MeshData2 meshVertices;

	float widthHeightDepth = 0.101025f; //0.06735f; // 0.1275f; //0.046875f; //0.1275f;

	char colorMode = 'x';
	XMMATRIX boxTrans = XMMatrixTranslation(1.0f, 0.0f, 0.0f);
	geoGen.CreateBox(0.5f, widthHeightDepth, widthHeightDepth, meshVertices, colorMode, boxTrans);

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * meshVertices.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vinitData.pSysMem = &meshVertices.Vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransTool_xAxisBox_VB));

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * meshVertices.Indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    vinitData.pSysMem = &meshVertices.Indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &vinitData, &mMeshTransTool_axisBox_IB));

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

	xScalingAxisHandle = new Handle_ScalingAxis(XMLoadFloat3(&xDir), listOfTrianglesAsPoints, 'x');
	listOfTrianglesAsPoints.clear();

	meshVertices.Vertices.clear();

		colorMode = 'x';
		boxTrans = XMMatrixTranslation(-1.0f, 0.0f, 0.0f);
		geoGen.CreateBox(0.5f, widthHeightDepth, widthHeightDepth, meshVertices, colorMode, boxTrans);

		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex::PosCol) * meshVertices.Vertices.size();
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vinitData.pSysMem = &meshVertices.Vertices[0];
		HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransTool_xAxisBox2_VB));

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

		xScalingAxisHandle2 = new Handle_ScalingAxis(XMLoadFloat3(&xDir), listOfTrianglesAsPoints, 'x');
		listOfTrianglesAsPoints.clear();

		meshVertices.Vertices.clear();

	// Y boxes.

	colorMode = 'y';
	boxTrans = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
	geoGen.CreateBox(widthHeightDepth, 0.5f, widthHeightDepth, meshVertices, colorMode, boxTrans);

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * meshVertices.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vinitData.pSysMem = &meshVertices.Vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransTool_yAxisBox_VB));

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

	yScalingAxisHandle = new Handle_ScalingAxis(XMLoadFloat3(&yDir), listOfTrianglesAsPoints, 'y');
	listOfTrianglesAsPoints.clear();

	meshVertices.Vertices.clear();

		colorMode = 'y';
		boxTrans = XMMatrixTranslation(0.0f, -1.0f, 0.0f);
		geoGen.CreateBox(widthHeightDepth, 0.5f, widthHeightDepth, meshVertices, colorMode, boxTrans);

		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex::PosCol) * meshVertices.Vertices.size();
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vinitData.pSysMem = &meshVertices.Vertices[0];
		HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransTool_yAxisBox2_VB));

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
		
		yScalingAxisHandle2 = new Handle_ScalingAxis(XMLoadFloat3(&yDir), listOfTrianglesAsPoints, 'y');
		listOfTrianglesAsPoints.clear();

		meshVertices.Vertices.clear();

	// Z boxes.

	colorMode = 'z';
	boxTrans = XMMatrixTranslation(0.0f, 0.0f, 1.0f);
	geoGen.CreateBox(widthHeightDepth, widthHeightDepth, 0.5f, meshVertices, colorMode, boxTrans);

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * meshVertices.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vinitData.pSysMem = &meshVertices.Vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransTool_zAxisBox_VB));

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

	zScalingAxisHandle = new Handle_ScalingAxis(XMLoadFloat3(&zDir), listOfTrianglesAsPoints, 'z');
	listOfTrianglesAsPoints.clear();

	meshVertices.Vertices.clear();

		colorMode = 'z';
		boxTrans = XMMatrixTranslation(0.0f, 0.0f, -1.0f);
		geoGen.CreateBox(widthHeightDepth, widthHeightDepth, 0.5f, meshVertices, colorMode, boxTrans);

		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex::PosCol) * meshVertices.Vertices.size();
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vinitData.pSysMem = &meshVertices.Vertices[0];
		HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransTool_zAxisBox2_VB));

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

		zScalingAxisHandle2 = new Handle_ScalingAxis(XMLoadFloat3(&zDir), listOfTrianglesAsPoints, 'z');
		listOfTrianglesAsPoints.clear();

		meshVertices.Vertices.clear();

	colorMode = 'o';
	boxTrans = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	geoGen.CreateBox(0.25f, 0.25f, 0.25f, meshVertices, colorMode, boxTrans);

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * meshVertices.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vinitData.pSysMem = &meshVertices.Vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransTool_omniAxisBox_VB));

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

	omniScalingAxisHandle = new Handle_OmniDimensionalScaling(listOfTrianglesAsPoints);
	listOfTrianglesAsPoints.clear();

	meshVertices.Vertices.clear();

	D3D11_BLEND_DESC blendDesc;
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_BLEND_FACTOR;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	md3dDevice->CreateBlendState(&blendDesc, &m_blendState);
}

void Tool_Scaling::draw(XMMATRIX &camView, XMMATRIX &camProj, ID3D11DepthStencilView *depthStencilView)
{
	// Draw the scaling tool...
	md3dImmediateContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	md3dImmediateContext->PSSetShader(m_pixelShader, 0, 0);
	md3dImmediateContext->VSSetShader(m_vertexShader, 0, 0);

	md3dImmediateContext->IASetInputLayout(m_inputLayout);
   
	UINT stride = sizeof(Vertex::PosCol);
    UINT offset = 0;
	
	XMVECTOR rotQuat = activeEntity->fetchData<Data::Transform>()->rotation;
	XMMATRIX rotation = XMMatrixRotationQuaternion(rotQuat);

	XMFLOAT4X4 toolWorld = getWorld_visual();
	XMMATRIX world = XMLoadFloat4x4(&toolWorld);
	
	XMMATRIX worldViewProj = world * camView * camProj;
	worldViewProj = XMMatrixTranspose(worldViewProj);

	md3dImmediateContext->UpdateSubresource(m_WVPBuffer, 0, NULL, &worldViewProj, 0, 0);
	md3dImmediateContext->UpdateSubresource(m_ColorSchemeIdBuffer, 0, NULL, &SETTINGS()->m_ColorScheme_3DManipulatorWidgets, 0, 0);
	ID3D11Buffer *buffers[2] = {m_WVPBuffer, m_ColorSchemeIdBuffer};
	md3dImmediateContext->VSSetConstantBuffers(0, 2, buffers);

	md3dImmediateContext->PSSetConstantBuffers(0, 1, &m_ToolPS_PerFrame_Buffer);
	Entity* entity_camera = CAMERA_ENTITY().asEntity();
	XMVECTOR camViewVector = entity_camera->fetchData<Data::Camera>()->getLookVector();
	//camViewVector = XMVector3Transform(camViewVector, worldViewProj);
	md3dImmediateContext->UpdateSubresource(m_ToolPS_PerFrame_Buffer, 0, NULL, &camViewVector, 0, 0);
	XMVECTOR toolPos = activeEntity->fetchData<Data::Transform>()->position;
	md3dImmediateContext->UpdateSubresource(m_ToolPS_PerFrame_Buffer, 1, NULL, &toolPos, 0, 0);


	// Draw control boxes.
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	md3dImmediateContext->IASetIndexBuffer(mMeshTransTool_axisBox_IB, DXGI_FORMAT_R32_UINT, offset);

	// Omni-scaling box.
	if(!isSelected || currentlySelectedHandle == omniScalingAxisHandle)
	{
		UINT sampleMask   = 0x0fffffff;
		md3dImmediateContext->OMSetBlendState(m_blendState, NULL, sampleMask);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_omniAxisBox_VB, &stride, &offset);
		md3dImmediateContext->DrawIndexed(36, 0, 0);

		float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.01f };
		md3dImmediateContext->OMSetBlendState(NULL, blendFactor, sampleMask);
	}

	// Draw control frames.

	if(!isSelected)
	{
		md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				
		UINT sampleMask   = 0x0fffffff;
		md3dImmediateContext->OMSetBlendState(m_blendState, NULL, sampleMask);

		md3dImmediateContext->UpdateSubresource(m_WVPBuffer, 0, NULL, &worldViewProj, 0, 0);
		md3dImmediateContext->VSSetConstantBuffers(0, 1, &m_WVPBuffer);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_obscuringRectangles_VB, &stride, &offset);
		md3dImmediateContext->Draw(72, 0);
			
		float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.01f };
		md3dImmediateContext->OMSetBlendState(NULL, blendFactor, sampleMask);
	}

	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	
	if(!isSelected || currentlySelectedPlane == yzScalingPlane || currentlySelectedPlane == yzScalingPlane2)
	{
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_yzPlane_VB, &stride, &offset);
		md3dImmediateContext->Draw(5, 0);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_yzPlane2_VB, &stride, &offset);
		md3dImmediateContext->Draw(5, 0);
	}

	if(!isSelected || currentlySelectedPlane == zxScalingPlane || currentlySelectedPlane == zxScalingPlane2)
	{
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_zxPlane_VB, &stride, &offset);
		md3dImmediateContext->Draw(5, 0);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_zxPlane2_VB, &stride, &offset);
		md3dImmediateContext->Draw(5, 0);
	}

	if(!isSelected || currentlySelectedPlane == xyScalingPlane || currentlySelectedPlane == xyScalingPlane2)
	{
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_xyPlane_VB, &stride, &offset);
		md3dImmediateContext->Draw(5, 0);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_xyPlane2_VB, &stride, &offset);
		md3dImmediateContext->Draw(5, 0);
	}

	// Draw control boxes.
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	md3dImmediateContext->IASetIndexBuffer(mMeshTransTool_axisBox_IB, DXGI_FORMAT_R32_UINT, offset);

	md3dImmediateContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Omni-scaling box.
	//if(!isSelected || currentlySelectedHandle == omniScalingAxisHandle)
	//{
	//	UINT sampleMask   = 0x0fffffff;
	//	md3dImmediateContext->OMSetBlendState(m_blendState, NULL, sampleMask);

	//	md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_omniAxisBox_VB, &stride, &offset);
	//	md3dImmediateContext->DrawIndexed(36, 0, 0);

	//	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.01f };
	//	md3dImmediateContext->OMSetBlendState(NULL, blendFactor, sampleMask);
	//}

	md3dImmediateContext->OMSetDepthStencilState(NULL, 0);

	// The other boxes...

	if(!isSelected || currentlySelectedAxis == xScalingAxisHandle
				   || currentlySelectedAxis == xScalingAxisHandle2
				   || currentlySelectedPlane == xyScalingPlane
				   || currentlySelectedPlane == xyScalingPlane2
				   || currentlySelectedPlane == zxScalingPlane
				   || currentlySelectedPlane == zxScalingPlane2)
	{
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_xAxisBox_VB, &stride, &offset);
		md3dImmediateContext->DrawIndexed(36, 0, 0);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_xAxisBox2_VB, &stride, &offset);
		md3dImmediateContext->DrawIndexed(36, 0, 0);
	}

	if(!isSelected || currentlySelectedAxis == yScalingAxisHandle
				   || currentlySelectedAxis == yScalingAxisHandle2
				   || currentlySelectedPlane == yzScalingPlane
				   || currentlySelectedPlane == yzScalingPlane2
				   || currentlySelectedPlane == xyScalingPlane
				   || currentlySelectedPlane == xyScalingPlane2)
	{
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_yAxisBox_VB, &stride, &offset);
		md3dImmediateContext->DrawIndexed(36, 0, 0);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_yAxisBox2_VB, &stride, &offset);
		md3dImmediateContext->DrawIndexed(36, 0, 0);
	}

	if(!isSelected || currentlySelectedAxis == zScalingAxisHandle
				   || currentlySelectedAxis == zScalingAxisHandle2
				   || currentlySelectedPlane == zxScalingPlane
				   || currentlySelectedPlane == zxScalingPlane2
				   || currentlySelectedPlane == yzScalingPlane
				   || currentlySelectedPlane == yzScalingPlane2)
	{
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_zAxisBox_VB, &stride, &offset);
		md3dImmediateContext->DrawIndexed(36, 0, 0);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_zAxisBox2_VB, &stride, &offset);
		md3dImmediateContext->DrawIndexed(36, 0, 0);
	}
}