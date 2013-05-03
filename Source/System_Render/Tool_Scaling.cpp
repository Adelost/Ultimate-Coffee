#include "stdafx.h"
#include "Tool_Scaling.h"

Tool_Scaling::Tool_Scaling()
{
	isSelected = false;
	currentlySelectedAxis = NULL;
	currentlySelectedPlane = NULL;

	XMFLOAT3 xDir(1.0f, 0.0f, 0.0f);
	XMFLOAT3 yDir(0.0f, 1.0f, 0.0f);
	XMFLOAT3 zDir(0.0f, 0.0f, 1.0f);
	XMFLOAT3 zDirNeg(0.0f, 0.0f, -1.0f);

	xTranslationAxisHandle = new Handle_TranslationAxis(XMLoadFloat3(&xDir));
	yTranslationAxisHandle = new Handle_TranslationAxis(XMLoadFloat3(&yDir));
	zTranslationAxisHandle = new Handle_TranslationAxis(XMLoadFloat3(&zDir));

	MyRectangle boundingRectangle;

	boundingRectangle.P1 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	boundingRectangle.P2 = XMFLOAT3(0.0f, 1.0f, 0.0f);
	boundingRectangle.P3 = XMFLOAT3(0.0f, 1.0f, 1.0f);
	boundingRectangle.P4 = XMFLOAT3(0.0f, 0.0f, 1.0f);
	yzTranslationPlane = new Handle_TranslationPlane(XMLoadFloat3(&xDir), 0.0f, boundingRectangle);

	boundingRectangle.P1 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	boundingRectangle.P2 = XMFLOAT3(0.0f, 0.0f, 1.0f);
	boundingRectangle.P3 = XMFLOAT3(1.0f, 0.0f, 1.0f);
	boundingRectangle.P4 = XMFLOAT3(1.0f, 0.0f, 0.0f);
	zxTranslationPlane = new Handle_TranslationPlane(XMLoadFloat3(&yDir), 0.0f, boundingRectangle);

	boundingRectangle.P1 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	boundingRectangle.P2 = XMFLOAT3(0.0f, 1.0f, 0.0f);
	boundingRectangle.P3 = XMFLOAT3(1.0f, 1.0f, 0.0f);
	boundingRectangle.P4 = XMFLOAT3(1.0f, 0.0f, 0.0f);
	xyTranslationPlane = new Handle_TranslationPlane(XMLoadFloat3(&zDir), 0.0f, boundingRectangle);

	boundingRectangle.P1 = XMFLOAT3(-0.25f, -0.25f, 0.0f);
	boundingRectangle.P2 = XMFLOAT3(-0.25f,  0.25f, 0.0f);
	boundingRectangle.P3 = XMFLOAT3( 0.25f,  0.25f, 0.0f);
	boundingRectangle.P4 = XMFLOAT3( 0.25f, -0.25f, 0.0f);
	camViewTranslationPlane = new Handle_TranslationPlane(XMLoadFloat3(&zDirNeg), 0.0f, boundingRectangle);

	relateToActiveObjectWorld = false;

	scale = 1.0f;
}

Tool_Scaling::~Tool_Scaling()
{
	delete xTranslationAxisHandle;
	delete yTranslationAxisHandle;
	delete zTranslationAxisHandle;
}

void Tool_Scaling::setIsVisible(bool &isVisible)
{
	this->isVisible = isVisible;
}

/* Called for an instance of picking, possibly resulting in the tool being selected. */
bool Tool_Scaling::tryForSelection(XMVECTOR &rayOrigin, XMVECTOR &rayDir, Camera &theCamera)
{

	bool aTranslationToolHandleWasSelected = false;

	bool rayIntersectsWithToolBoundingBox = false;
	// Check if the picking ray intersects with any of the translation tool's bounding box.
	// ...
	// TEST: Pretend we hit it.
	rayIntersectsWithToolBoundingBox = true;
	
	if(rayIntersectsWithToolBoundingBox)
	{
		// Check if the ray intersects with any of the control handles.
		
		float distanceToPointOfIntersection;
		
		bool planeSelected = camViewTranslationPlane->tryForSelection(rayOrigin, rayDir, theCamera.View(), distanceToPointOfIntersection);

		// If the camera view translation plane is intersected, it is always selected...
		if(planeSelected)
		{
			currentlySelectedPlane = camViewTranslationPlane;
			aTranslationToolHandleWasSelected = true;
		}
		else // ... Else, the others are tested for intersection and prioritized by intersection point distance:
		{
			float distanceToClosestPointOfIntersection = FLT_MAX;

			planeSelected = xyTranslationPlane->tryForSelection(rayOrigin, rayDir, theCamera.View(), distanceToPointOfIntersection);
			if(planeSelected)
			{
				distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
				currentlySelectedPlane = xyTranslationPlane;
				aTranslationToolHandleWasSelected = true;
			}

			planeSelected = yzTranslationPlane->tryForSelection(rayOrigin, rayDir, theCamera.View(), distanceToPointOfIntersection);
			if(planeSelected)
			{
				if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
				{
					distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
					currentlySelectedPlane = yzTranslationPlane;
					aTranslationToolHandleWasSelected = true;
				}
			}

			planeSelected = zxTranslationPlane->tryForSelection(rayOrigin, rayDir, theCamera.View(), distanceToPointOfIntersection);
			if(planeSelected)
			{
				if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
				{
					distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
					currentlySelectedPlane = zxTranslationPlane;
					aTranslationToolHandleWasSelected = true;
				}
			}
		}
	}
	
	isSelected = aTranslationToolHandleWasSelected;
	return aTranslationToolHandleWasSelected;
}

/* Called to bind the translatable object to the tool, so its translation can be modified. */
void Tool_Scaling::setActiveObject(IObject *object)
{
	this->activeObject = object;

	// Set the visual and bounding components of the translation tool to the pivot point of the active object.
	updateWorld();

	XMStoreFloat4x4(&originalWorldOfActiveObject, object->getIRenderable()->getWorld());
}

void Tool_Scaling::updateWorld()
{
	if(!relateToActiveObjectWorld)
	{
		// Just get the position of the active object, but keep the default orientation.
		XMMATRIX newWorld = XMMatrixIdentity();

		XMFLOAT4X4 objectWorld;
		XMStoreFloat4x4(&objectWorld, activeObject->getIRenderable()->getWorld());

		newWorld._41 = objectWorld._41;
		newWorld._42 = objectWorld._42;
		newWorld._43 = objectWorld._43;

		// Update the translation tool's (distance-from-camera-adjusted) scale.
		//newWorld._11 = scale;
		//newWorld._22 = scale;
		//newWorld._33 = scale;
		XMStoreFloat4x4(&world, newWorld);

		XMMATRIX logicalWorld = XMLoadFloat4x4(&getWorld_logical());

		// Transform all the controls.
		xTranslationAxisHandle->setWorld(logicalWorld);
		yTranslationAxisHandle->setWorld(logicalWorld);
		zTranslationAxisHandle->setWorld(logicalWorld);

		yzTranslationPlane->setWorld(logicalWorld);
		zxTranslationPlane->setWorld(logicalWorld);
		xyTranslationPlane->setWorld(logicalWorld);

		camViewTranslationPlane->setWorld(XMLoadFloat4x4(&getWorld_viewPlaneTranslationControl_logical()));
	}
	else
	{
		// Get the position and orientation of the active object.
		XMStoreFloat4x4(&world, activeObject->getIRenderable()->getWorld());

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

		yzTranslationPlane->setWorld(logicalWorld);
		zxTranslationPlane->setWorld(logicalWorld);
		xyTranslationPlane->setWorld(logicalWorld);
	}
}

/* Transform all controls to the local coord. sys. of the active object. */
void Tool_Scaling::setRelateToActiveObjectWorld(bool relateToActiveObjectWorld)
{
	this->relateToActiveObjectWorld = relateToActiveObjectWorld;

	if(activeObject)
		updateWorld();
}

/* Called to see if the translation tool is (still) active. */
bool Tool_Scaling::getIsSelected()
{
	return isSelected;
}

/* Called to send updated parameters to the translation tool, if it is still active. */
void Tool_Scaling::update(XMVECTOR &rayOrigin, XMVECTOR &rayDir, Camera &theCamera, D3D11_VIEWPORT &theViewport, POINT &mouseCursorPoint)
{
	if(currentlySelectedAxis)
	{
		// Pick against the plane to update the translation delta.
		currentlySelectedAxis->pickAxisPlane(rayOrigin);

		translateObject();
	}
	else if(currentlySelectedPlane)
	{
		// 
		currentlySelectedPlane->pickPlane(rayOrigin, rayDir, theCamera.View());

		//XMMATRIX curWorld = activeObject->getIRenderable()->getWorld();

		//XMVECTOR newPos;
		//newPos.m128_f32[0] = curWorld._41;
		//newPos.m128_f32[1] = curWorld._42;
		//newPos.m128_f32[2] = curWorld._43;

		//activeObject->getIRenderable()->incrementTranslation(currentlySelectedPlane->getLastTranslationDelta());

		XMVECTOR transDelta = currentlySelectedPlane->getLastTranslationDelta();

		//XMFLOAT4 newPos;
		//newPos.x = originalWorldOfActiveObject._41 + transDelta.m128_f32[0];
		//newPos.y = originalWorldOfActiveObject._42 + transDelta.m128_f32[1];
		//newPos.z = originalWorldOfActiveObject._43 + transDelta.m128_f32[2];

		float scaleFactor = scale;
	/*	if(currentlySelectedPlane == camViewTranslationPlane)
			scaleFactor = 1.0f;*/

		XMFLOAT4X4 newMatrix; //  = XMMatrixIdentity();
		newMatrix = originalWorldOfActiveObject;

		if(currentlySelectedPlane != camViewTranslationPlane)
		{
			newMatrix._41 = originalWorldOfActiveObject._41 + transDelta.m128_f32[0] * scaleFactor;
			newMatrix._42 = originalWorldOfActiveObject._42 + transDelta.m128_f32[1] * scaleFactor;
			newMatrix._43 = originalWorldOfActiveObject._43 + transDelta.m128_f32[2] * scaleFactor;
		}
		else
		{

			//XMVECTOR test = 
			//	
			//	XMLoadFloat3((XMFLOAT3*)world_viewPlaneTranslationControl_logical.m[0]) * transDelta.m128_f32[0]  
			//	+
			//	XMLoadFloat3((XMFLOAT3*)world_viewPlaneTranslationControl_logical.m[1]) * transDelta.m128_f32[1] ;

			//originalWorldOfActiveObject._41 += test.m128_f32[0];
			//originalWorldOfActiveObject._42 += test.m128_f32[1];
			//originalWorldOfActiveObject._43 += test.m128_f32[2];
		}

		XMFLOAT4X4 newM;
		//XMStoreFloat4x4(&newM, newMatrix);
		activeObject->getIRenderable()->setWorld(newMatrix);
	
		//XMMATRIX newWorld = XMMatrixIdentity();
		//XMFLOAT4X4 objectWorld = *(activeObject->getIRenderable()->getWorld());
		//newWorld._41 = objectWorld._41;
		//newWorld._42 = objectWorld._42;
		//newWorld._43 = objectWorld._43;

		//world = newMatrix;

		//world._41 += transDelta.m128_f32[0];
		//world._42 += transDelta.m128_f32[1];
		//world._43 += transDelta.m128_f32[2];
	}
}

/* Called for current translation delta made by picking. */
void Tool_Scaling::translateObject()
{
	XMVECTOR lastTransDelta = currentlySelectedAxis->getLastTranslationDelta();
	((Object_Basic*)activeObject)->getIRenderable()->incrementTranslation(lastTransDelta);
}

/* Called when the translation tool is unselected, which makes any hitherto made translation final (and undoable). */
void Tool_Scaling::unselect()
{
	// Set the controls' visual and bounding components to the active object's new position and orientation.
	updateWorld();

	currentlySelectedPlane = NULL;
	currentlySelectedAxis = NULL;

	isSelected = false;
}

XMFLOAT4X4 Tool_Scaling::getWorld_logical()
{
	XMMATRIX trans = XMMatrixTranslation(world._41, world._42, world._43);

	XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);

	XMFLOAT4X4 world;
	XMStoreFloat4x4(&world, scaling * trans);

	return world;
}

XMFLOAT4X4 Tool_Scaling::getWorld_visual()
{
	XMMATRIX trans = XMMatrixTranslation(activeObject->getIRenderable()->getWorld()._41, activeObject->getIRenderable()->getWorld()._42, activeObject->getIRenderable()->getWorld()._43);

	XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);

	XMFLOAT4X4 world_visual;
	XMStoreFloat4x4(&world_visual, scaling * trans);

	return world_visual;
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

	world_viewPlaneTranslationControl_visual._41 = activeObject->getIRenderable()->getWorld().m[3][0];
	world_viewPlaneTranslationControl_visual._42 = activeObject->getIRenderable()->getWorld().m[3][1];
	world_viewPlaneTranslationControl_visual._43 = activeObject->getIRenderable()->getWorld().m[3][2];
	world_viewPlaneTranslationControl_visual._44 = activeObject->getIRenderable()->getWorld().m[3][3];
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

IObject *Tool_Scaling::getActiveObject()
{
	return activeObject;
}
	
void init(ID3D11Device *device, ID3D11DeviceContext *deviceContext)
{
}

void draw()
{
}