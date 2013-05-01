#include "Tool_Rotation.h"

Tool_Rotation::Tool_Rotation(HWND windowHandle)
{
	isSelected = false;
	currentlySelectedHandle = NULL;

	scale = 1.0f;
	XMVECTOR center = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	omniRotateSphereHandle = new Handle_RotationSphere(center, scale, windowHandle);

	relateToActiveObjectWorld = false;
}

Tool_Rotation::~Tool_Rotation()
{
	delete omniRotateSphereHandle;
}

void Tool_Rotation::setIsVisible(bool &isVisible)
{
	this->isVisible = isVisible;
}

/* Called for an instance of picking, possibly resulting in the tool being selected. */
bool Tool_Rotation::tryForSelection(XMVECTOR &rayOrigin, XMVECTOR &rayDir, Camera &theCamera)
{
	bool aRotationToolHandleWasSelected = false;

	bool rayIntersectsWithToolBoundingBox = false;

	// Check if the picking ray intersects with any of the translation tool's bounding box.
	// ...
	// TEST: Pretend we hit it.
	rayIntersectsWithToolBoundingBox = true;
	
	if(rayIntersectsWithToolBoundingBox)
	{
		// Check if the ray intersects with any of the control handles.
		
		float distanceToPointOfIntersection;
		
		// Check if the ray intersects with the omni-rotation sphere.
		bool sphereSelected = omniRotateSphereHandle->tryForSelection(rayOrigin, rayDir, theCamera.View(), distanceToPointOfIntersection);
		if(sphereSelected)
		{
			currentlySelectedHandle = omniRotateSphereHandle;
			aRotationToolHandleWasSelected = true;
		}
	}
	
	isSelected = aRotationToolHandleWasSelected;
	return aRotationToolHandleWasSelected;
}

/* Called to bind the translatable object to the tool, so its translation can be modified. */
void Tool_Rotation::setActiveObject(IObject *object)
{
	this->activeObject = object;

	// Set the visual and bounding components of the translation tool to the pivot point of the active object.
	updateWorld();

	XMStoreFloat4x4(&originalWorldOfActiveObject, object->getIRenderable()->getWorld());

	XMStoreFloat4(&originalRotationQuatOfActiveObject, object->getIRenderable()->getRotation());

	omniRotateSphereHandle->resetRotationQuaternion();
}

void Tool_Rotation::updateWorld()
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

		// Update the rotation tool's (distance-from-camera-adjusted) scale.
		//newWorld._11 = scale;
		//newWorld._22 = scale;
		//newWorld._33 = scale;
		XMStoreFloat4x4(&world, newWorld);

		XMMATRIX logicalWorld = XMLoadFloat4x4(&getWorld_logical());

		// Transform all the controls.
		omniRotateSphereHandle->setWorld(logicalWorld);
	}
	else
	{
		// Just get the position of the active object, but keep the default orientation.
		XMMATRIX newWorld = XMMatrixIdentity();
		XMFLOAT4X4 objectWorld;
		XMStoreFloat4x4(&objectWorld, activeObject->getIRenderable()->getWorld());
		newWorld._41 = objectWorld._41;
		newWorld._42 = objectWorld._42;
		newWorld._43 = objectWorld._43;

		// Update the rotation tool's (distance-from-camera-adjusted) scale.
		//newWorld._11 = scale;
		//newWorld._22 = scale;
		//newWorld._33 = scale;
		XMStoreFloat4x4(&world, newWorld);

		XMMATRIX logicalWorld = XMLoadFloat4x4(&getWorld_logical());

		// Transform all the controls.
		omniRotateSphereHandle->setWorld(logicalWorld);
	}
}

/* Transform all controls to the local coord. sys. of the active object. */
void Tool_Rotation::setRelateToActiveObjectWorld(bool relateToActiveObjectWorld)
{
	this->relateToActiveObjectWorld = relateToActiveObjectWorld;

	if(activeObject)
		updateWorld();
}

/* Called to see if the translation tool is (still) active. */
bool Tool_Rotation::getIsSelected()
{
	return isSelected;
}

/* Called to send updated parameters to the translation tool, if it is still active. */
void Tool_Rotation::update(XMVECTOR &rayOrigin, XMVECTOR &rayDir, Camera &theCamera, D3D11_VIEWPORT &theViewport, POINT &mouseCursorPoint)
{
	if(currentlySelectedHandle)
	{
		// Pick against the plane to update the translation delta.
		if(currentlySelectedHandle = omniRotateSphereHandle)
		{
			omniRotateSphereHandle->pickSphere(rayOrigin, rayDir, theCamera, theViewport, mouseCursorPoint);

			XMVECTOR rotQuaternion = omniRotateSphereHandle->getTotalRotationQuaternion(); //getLastRotationQuaternion();
			
			XMVECTOR newRotQuat = XMQuaternionMultiply(XMLoadFloat4(&originalRotationQuatOfActiveObject), rotQuaternion);

			activeObject->getIRenderable()->setRotation(newRotQuat);

				//activeObject->getIRenderable()->rotate(rotQuaternion);

					//XMMATRIX rot = XMMatrixRotationQuaternion(rotQuaternion);

					//XMFLOAT4X4 newMatrix;
					//XMStoreFloat4x4(&newMatrix, rot * XMLoadFloat4x4(&originalWorldOfActiveObject));

					//activeObject->getIRenderable()->setWorld(newMatrix);
		}
	}
}

/* Called for current translation delta made by picking. */
void Tool_Rotation::translateObject()
{
	//XMVECTOR lastTransDelta = currentlySelectedAxis->getLastTranslationDelta();
	//((Object_Basic*)activeObject)->getIRenderable()->incrementTranslation(lastTransDelta);
}

/* Called when the translation tool is unselected, which makes any hitherto made translation final (and undoable). */
void Tool_Rotation::unselect()
{
	// Set the controls' visual and bounding components to the active object's new position and orientation.
	updateWorld();

	currentlySelectedHandle = NULL;

	isSelected = false;
}

XMFLOAT4X4 Tool_Rotation::getWorld_logical()
{
	XMMATRIX trans = XMMatrixTranslation(world._41, world._42, world._43);

	XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);

	XMFLOAT4X4 world;
	XMStoreFloat4x4(&world, scaling * trans);

	return world;
}

XMFLOAT4X4 Tool_Rotation::getWorld_visual()
{
	XMMATRIX trans = XMMatrixTranslation(activeObject->getIRenderable()->getWorld()._41, activeObject->getIRenderable()->getWorld()._42, activeObject->getIRenderable()->getWorld()._43);

	XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);

	XMFLOAT4X4 world_visual;
	XMStoreFloat4x4(&world_visual, scaling * trans);

	return world_visual;
}

void Tool_Rotation::setScale(float &scale)
{
	this->scale = scale;
}

float Tool_Rotation::getScale()
{
	return scale;
}

/* Called to see if the visual component of the translation tool should be rendered. */
bool Tool_Rotation::getIsVisible()
{
	return isVisible;
}

void Tool_Rotation::updateViewPlaneTranslationControlWorld(XMFLOAT3 &camViewVector, XMFLOAT3 &camUpVector)
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

XMFLOAT4X4 Tool_Rotation::getWorld_viewPlaneTranslationControl_logical()
{
	XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);

	XMMATRIX loadedWorld = XMLoadFloat4x4(&world_viewPlaneTranslationControl_logical);
	XMFLOAT4X4 logicalWorld;
	XMStoreFloat4x4(&logicalWorld, scaling * loadedWorld);

	return logicalWorld;
}

XMFLOAT4X4 Tool_Rotation::getWorld_viewPlaneTranslationControl_visual()
{
	XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);

	XMMATRIX loadedWorld = XMLoadFloat4x4(&world_viewPlaneTranslationControl_visual);
	XMFLOAT4X4 visualWorld;
	XMStoreFloat4x4(&visualWorld, scaling * loadedWorld);

	return visualWorld;
}

IObject *Tool_Rotation::getActiveObject()
{
	return activeObject;
}
	
void init(ID3D11Device *device, ID3D11DeviceContext *deviceContext)
{

	SETTINGS()->selectedTool;
}

void draw()
{
	// Draw control circles.

	XMVECTOR rotQuat = testObject->getIRenderable()->getRotation();
	XMMATRIX rotation = XMMatrixRotationQuaternion(rotQuat);

	XMFLOAT4X4 toolWorld = currentlySelectedTransformationTool->getWorld_visual();
	XMMATRIX world = XMLoadFloat4x4(&toolWorld);
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
	worldViewProj = rotation*world*view*proj;

	Effects::ToolFX->SetWorldViewProj(worldViewProj);

	md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshRotTool_Xcircle_VB, &stride, &offset);
	activeMeshTech->GetPassByIndex(0)->Apply(0, md3dImmediateContext);
	md3dImmediateContext->Draw(65, 0);

	md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshRotTool_Ycircle_VB, &stride, &offset);
	activeMeshTech->GetPassByIndex(0)->Apply(0, md3dImmediateContext);
	md3dImmediateContext->Draw(65, 0);

	md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshRotTool_Zcircle_VB, &stride, &offset);
	activeMeshTech->GetPassByIndex(0)->Apply(0, md3dImmediateContext);
	md3dImmediateContext->Draw(65, 0);
}