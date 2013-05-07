#include "stdafx.h"
#include "Tool_Scaling.h"

#include <Core/Data.h>
#include <Core/DataMapper.h>
#include <Core/Events.h>

//#include <Core/Command_TranslateSceneEntity.h>

Tool_Scaling::Tool_Scaling(/*HWND windowHandle*/)
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
	yzScalingPlane = new Handle_ScalingPlane(XMLoadFloat3(&xDir), 0.0f, boundingRectangle, /*windowHandle,*/ YZ);

	boundingRectangle.P1 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	boundingRectangle.P2 = XMFLOAT3(0.0f, 0.0f, 1.0f);
	boundingRectangle.P3 = XMFLOAT3(1.0f, 0.0f, 1.0f);
	boundingRectangle.P4 = XMFLOAT3(1.0f, 0.0f, 0.0f);
	zxScalingPlane = new Handle_ScalingPlane(XMLoadFloat3(&yDir), 0.0f, boundingRectangle, /*windowHandle,*/ XZ);

	boundingRectangle.P1 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	boundingRectangle.P2 = XMFLOAT3(0.0f, 1.0f, 0.0f);
	boundingRectangle.P3 = XMFLOAT3(1.0f, 1.0f, 0.0f);
	boundingRectangle.P4 = XMFLOAT3(1.0f, 0.0f, 0.0f);
	xyScalingPlane = new Handle_ScalingPlane(XMLoadFloat3(&zDir), 0.0f, boundingRectangle, /*windowHandle,*/ XY);

	boundingRectangle.P1 = XMFLOAT3(-0.25f, -0.25f, 0.0f);
	boundingRectangle.P2 = XMFLOAT3(-0.25f,  0.25f, 0.0f);
	boundingRectangle.P3 = XMFLOAT3( 0.25f,  0.25f, 0.0f);
	boundingRectangle.P4 = XMFLOAT3( 0.25f, -0.25f, 0.0f);
	camViewScalingPlane = new Handle_ScalingPlane(XMLoadFloat3(&zDirNeg), 0.0f, boundingRectangle, /*windowHandle,*/ YZ);

	relateToActiveObjectWorld = false;

	shouldFlipMouseCursor = false;
	xyScalingPlane->setShouldFlipMouseCursor(shouldFlipMouseCursor);
	zxScalingPlane->setShouldFlipMouseCursor(shouldFlipMouseCursor);
	yzScalingPlane->setShouldFlipMouseCursor(shouldFlipMouseCursor);

	scale = 1.0f;

	activeEntityId = -1;
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
bool Tool_Scaling::tryForSelection(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView)
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
		
		bool planeSelected = camViewScalingPlane->tryForSelection(rayOrigin, rayDir, camView, distanceToPointOfIntersection);

		// If the camera view translation plane is intersected, it is always selected...
		if(planeSelected)
		{
			currentlySelectedPlane = camViewScalingPlane;
			aTranslationToolHandleWasSelected = true;
		}
		else // ... Else, the others are tested for intersection and prioritized by intersection point distance:
		{
			float distanceToClosestPointOfIntersection = FLT_MAX;

			planeSelected = xyScalingPlane->tryForSelection(rayOrigin, rayDir, camView, distanceToPointOfIntersection);
			if(planeSelected)
			{
				distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
				currentlySelectedPlane = xyScalingPlane;
				aTranslationToolHandleWasSelected = true;
			}

			planeSelected = yzScalingPlane->tryForSelection(rayOrigin, rayDir, camView, distanceToPointOfIntersection);
			if(planeSelected)
			{
				if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
				{
					distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
					currentlySelectedPlane = yzScalingPlane;
					aTranslationToolHandleWasSelected = true;
				}
			}

			planeSelected = zxScalingPlane->tryForSelection(rayOrigin, rayDir, camView, distanceToPointOfIntersection);
			if(planeSelected)
			{
				if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
				{
					distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
					currentlySelectedPlane = zxScalingPlane;
					aTranslationToolHandleWasSelected = true;
				}
			}
		}
	}
	
	isSelected = aTranslationToolHandleWasSelected;
	return aTranslationToolHandleWasSelected;
}

void Tool_Scaling::tryForHover(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView)
{
}

/* Called to bind the translatable object to the tool, so its translation can be modified. */
void Tool_Scaling::setActiveObject(int entityId)
{
	this->activeEntityId = entityId;

	// Set the visual and bounding components of the translation tool to the pivot point of the active object.
	updateWorld();

	XMMATRIX world = Entity(activeEntityId).fetchData<Data::Transform>()->toWorldMatrix();

	xyScalingPlane->resetScalingDelta();
	zxScalingPlane->resetScalingDelta();
	yzScalingPlane->resetScalingDelta();

	XMStoreFloat4x4(&originalWorldOfActiveObject, world);
}

void Tool_Scaling::updateWorld()
{
	if(!relateToActiveObjectWorld)
	{
		// Just get the position of the active object, but keep the default orientation.
		Matrix newWorld = XMMatrixIdentity();

		Entity e(activeEntityId);
		Data::Transform* trans = e.fetchData<Data::Transform>();

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
		xTranslationAxisHandle->setWorld(logicalWorld);
		yTranslationAxisHandle->setWorld(logicalWorld);
		zTranslationAxisHandle->setWorld(logicalWorld);

		yzScalingPlane->setWorld(logicalWorld);
		zxScalingPlane->setWorld(logicalWorld);
		xyScalingPlane->setWorld(logicalWorld);

		camViewScalingPlane->setWorld(XMLoadFloat4x4(&getWorld_viewPlaneTranslationControl_logical()));
	}
	else
	{
		// Get the position and orientation of the active object.
		world = Entity(activeEntityId).fetchData<Data::Transform>()->toWorldMatrix();

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

		yzScalingPlane->setWorld(logicalWorld);
		zxScalingPlane->setWorld(logicalWorld);
		xyScalingPlane->setWorld(logicalWorld);
	}
}

/* Transform all controls to the local coord. sys. of the active object. */
void Tool_Scaling::setRelateToActiveObjectWorld(bool relateToActiveObjectWorld)
{
	this->relateToActiveObjectWorld = relateToActiveObjectWorld;

	if(activeEntityId != -1)
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
void Tool_Scaling::update(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, XMMATRIX &camProj, D3D11_VIEWPORT &theViewport, POINT &mouseCursorPoint)
{
	if(currentlySelectedAxis)
	{
		// Pick against the plane to update the translation delta.
		currentlySelectedAxis->pickAxisPlane(rayOrigin);
	}
	else if(currentlySelectedPlane)
	{
		currentlySelectedPlane->pickPlane(rayOrigin, rayDir, camView, camProj, theViewport);
		currentlySelectedPlane->calcLastScalingDelta();
		XMVECTOR transDelta = currentlySelectedPlane->getTotalScalingDelta();

		XMFLOAT4X4 newMatrix;
		newMatrix = originalWorldOfActiveObject;

		float toolScaleDependantScaleFactor = scale;

		float scaleDependantScaleFactorX = originalWorldOfActiveObject._11 * 0.1f;
		float scaleDependantScaleFactorY = originalWorldOfActiveObject._22 * 0.1f;
		float scaleDependantScaleFactorZ = originalWorldOfActiveObject._33 * 0.1f;
		if(scaleDependantScaleFactorX < 0.1f)
			scaleDependantScaleFactorX = 0.1f;
		if(scaleDependantScaleFactorY < 0.1f)
			scaleDependantScaleFactorY = 0.1f;
		if(scaleDependantScaleFactorZ < 0.1f)
			scaleDependantScaleFactorZ = 0.1f;

		if(currentlySelectedPlane != camViewScalingPlane)
		{
			newMatrix._11 = originalWorldOfActiveObject._11 + (transDelta.m128_f32[0] * (toolScaleDependantScaleFactor) * scaleDependantScaleFactorX);
			newMatrix._22 = originalWorldOfActiveObject._22 + (transDelta.m128_f32[1] * (toolScaleDependantScaleFactor) * scaleDependantScaleFactorY);
			newMatrix._33 = originalWorldOfActiveObject._33 + (transDelta.m128_f32[2] * (toolScaleDependantScaleFactor) * scaleDependantScaleFactorZ);

			if(newMatrix._11 < 0.01f)
				newMatrix._11 = 0.01f;
			if(newMatrix._22 < 0.01f)
				newMatrix._22 = 0.01f;
			if(newMatrix._33 < 0.01f)
				newMatrix._33 = 0.01f;
		}
		else
		{
		}

		Entity(activeEntityId).fetchData<Data::Transform>()->scale = Vector3(newMatrix._11, newMatrix._22, newMatrix._33);
	}
}

/* Called when the translation tool is unselected, which makes any hitherto made translation final (and undoable). */
void Tool_Scaling::unselect()
{
	// Set the controls' visual and bounding components to the active object's new position and orientation.
	updateWorld();

	if(currentlySelectedPlane)
	{
		currentlySelectedPlane->unselect();
		currentlySelectedPlane = NULL;
	}
	
	currentlySelectedAxis = NULL;

	//Command_ScaleSceneEntity *command = new Command_ScaleSceneEntity(activeEntityId);
	//Entity e(activeEntityId);
	//Data::Transform* trans = e.fetchData<Data::Transform>();
	//command->setDoScaling(trans->scale.x, trans->scale.y, trans->scale.z);
	//command->setUndoScaling(originalWorldOfActiveObject._11, originalWorldOfActiveObject._22, originalWorldOfActiveObject._33);
	//SEND_EVENT(&Event_StoreCommandInCommandHistory(command, false)); 

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
	XMVECTOR trans = Entity(activeEntityId).fetchData<Data::Transform>()->position;
	XMMATRIX translation = XMMatrixTranslationFromVector(trans);

	XMMATRIX scaling;
	scaling = XMMatrixScaling(scale, scale, scale);

	XMFLOAT4X4 world_visual;
	XMStoreFloat4x4(&world_visual, scaling * translation);

	return world_visual;
}

XMFLOAT4X4 Tool_Scaling::getWorld_visual_objectRelative()
{
	XMVECTOR trans = Entity(activeEntityId).fetchData<Data::Transform>()->position;
	XMMATRIX translation = XMMatrixTranslationFromVector(trans);

	XMVECTOR scale = Entity(activeEntityId).fetchData<Data::Transform>()->scale;
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

	Vector3 activeEntityPos = Entity(activeEntityId).fetchData<Data::Transform>()->position;
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

int Tool_Scaling::getActiveObject()
{
	return activeEntityId;
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

	md3dImmediateContext->VSSetConstantBuffers(0, 1, &m_WVPBuffer);

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

		// XY triangle-list rectangle.
		posCol.Col.x = 0.0f; posCol.Col.y = 0.0f; posCol.Col.z = 0.0f; posCol.Col.w = 0.0f; // Transparent.

		// Triangle A...

		posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		posCol.Pos.x = 0.0f; posCol.Pos.y = 0.5f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		posCol.Pos.x = 0.5f; posCol.Pos.y = 0.5f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		// Triangle B...

		posCol.Pos.x = 0.5f; posCol.Pos.y = 0.5f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
		vertices.push_back(posCol);

		posCol.Pos.x = 0.5f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
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
}

void Tool_Scaling::draw(XMMATRIX &camView, XMMATRIX &camProj, ID3D11DepthStencilView *depthStencilView)
{
	// Draw the translation tool...
	md3dImmediateContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	md3dImmediateContext->PSSetShader(m_pixelShader, 0, 0);
	md3dImmediateContext->VSSetShader(m_vertexShader, 0, 0);

	md3dImmediateContext->IASetInputLayout(m_inputLayout);
   
	UINT stride = sizeof(Vertex::PosCol);
    UINT offset = 0;
	
	Entity e(activeEntityId);

	XMVECTOR rotQuat = e.fetchData<Data::Transform>()->rotation;;
	XMMATRIX rotation = XMMatrixRotationQuaternion(rotQuat);

	XMFLOAT4X4 toolWorld = getWorld_visual();
	XMMATRIX world = XMLoadFloat4x4(&toolWorld);
	
	XMMATRIX worldViewProj = world * camView * camProj;
	worldViewProj = XMMatrixTranspose(worldViewProj);

	ConstantBuffer2 WVP;
	WVP.WVP = worldViewProj;
	md3dImmediateContext->UpdateSubresource(m_WVPBuffer, 0, NULL, &WVP, 0, 0);
	md3dImmediateContext->VSSetConstantBuffers(0, 1, &m_WVPBuffer);

	// Draw control frames.

	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_yzPlane_VB, &stride, &offset);
	md3dImmediateContext->Draw(5, 0);

	md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_zxPlane_VB, &stride, &offset);
	md3dImmediateContext->Draw(5, 0);

	md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_xyPlane_VB, &stride, &offset);
	md3dImmediateContext->Draw(5, 0);

	//md3dImmediateContext->OMSetDepthStencilState(0, 0); // Perhaps unnecessary:
}