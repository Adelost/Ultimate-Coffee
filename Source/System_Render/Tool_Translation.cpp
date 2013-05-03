#include "stdafx.h"
#include "Tool_Translation.h"
#include <Core/Data.h>
#include <Core/DataMapper.h>

Tool_Translation::Tool_Translation()
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
	camViewTranslationPlane = new Handle_TranslationPlane(XMLoadFloat3(&zDirNeg), 0.0f, boundingRectangle);

	relateToActiveObjectWorld = false;

	activeEntityId = -1;

	scale = 1.0f;
}

Tool_Translation::~Tool_Translation()
{
	delete xTranslationAxisHandle;
	delete yTranslationAxisHandle;
	delete zTranslationAxisHandle;

	delete yzTranslationPlane; delete yzTranslationPlane2;
	delete zxTranslationPlane; delete zxTranslationPlane2;
	delete xyTranslationPlane; delete xyTranslationPlane2;
	delete camViewTranslationPlane;

	
}

void Tool_Translation::setIsVisible(bool &isVisible)
{
	this->isVisible = isVisible;
}

/* Called for an instance of picking, possibly resulting in the tool being selected. */
bool Tool_Translation::tryForSelection(XMVECTOR &rayOrigin, XMVECTOR &rayDir, Camera &theCamera)
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

				planeSelected = xyTranslationPlane2->tryForSelection(rayOrigin, rayDir, theCamera.View(), distanceToPointOfIntersection);
				if(planeSelected)
				{
					if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
					{
						distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
						currentlySelectedPlane = xyTranslationPlane2;
						aTranslationToolHandleWasSelected = true;
					}
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
			
				planeSelected = yzTranslationPlane2->tryForSelection(rayOrigin, rayDir, theCamera.View(), distanceToPointOfIntersection);
				if(planeSelected)
				{
					if(distanceToPointOfIntersection < distanceToClosestPointOfIntersection)
					{
						distanceToClosestPointOfIntersection = distanceToPointOfIntersection;
						currentlySelectedPlane = yzTranslationPlane2;
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

				planeSelected = zxTranslationPlane2->tryForSelection(rayOrigin, rayDir, theCamera.View(), distanceToPointOfIntersection);
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
	return aTranslationToolHandleWasSelected;
}

/* Called to bind the translatable object to the tool, so its translation can be modified. */
void Tool_Translation::setActiveObject(int entityId)
{
	this->activeEntityId = entityId;

	// Set the visual and bounding components of the translation tool to the pivot point of the active object.
	updateWorld();

	Matrix world = Entity(activeEntityId).fetchData<Data::Transform>()->toWorldMatrix();
	originalWorldOfActiveObject = static_cast<XMFLOAT4X4>(world);
	//XMStoreFloat4x4(&originalWorldOfActiveObject, world);
}

int Tool_Translation::getActiveObject()
{
	return activeEntityId;
}

void Tool_Translation::updateWorld()
{
	if(!relateToActiveObjectWorld)
	{
		// Just get the position of the active object, but keep the default orientation.
		Matrix newWorld = XMMatrixIdentity();

		XMFLOAT4X4 objectWorld;

		Entity e(activeEntityId);
		Data::Transform* trans = e.fetchData<Data::Transform>();
		objectWorld = static_cast<XMFLOAT4X4>(world);
		//XMStoreFloat4x4(&objectWorld, activeObject->getIRenderable()->getWorld());
		
		newWorld._41 = objectWorld._41;
		newWorld._42 = objectWorld._42;
		newWorld._43 = objectWorld._43;

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

		yzTranslationPlane->setWorld(logicalWorld);
		zxTranslationPlane->setWorld(logicalWorld);
		xyTranslationPlane->setWorld(logicalWorld);

		yzTranslationPlane2->setWorld(logicalWorld);
		zxTranslationPlane2->setWorld(logicalWorld);
		xyTranslationPlane2->setWorld(logicalWorld);

		camViewTranslationPlane->setWorld(XMLoadFloat4x4(&getWorld_viewPlaneTranslationControl_logical()));
	}
	else
	{
		world = Entity(activeEntityId).fetchData<Data::Transform>()->toWorldMatrix();

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

		yzTranslationPlane->setWorld(logicalWorld);
		zxTranslationPlane->setWorld(logicalWorld);
		xyTranslationPlane->setWorld(logicalWorld);
	}
}

/* Transform all controls to the local coord. sys. of the active object. */
void Tool_Translation::setRelateToActiveObjectWorld(bool relateToActiveObjectWorld)
{
	this->relateToActiveObjectWorld = relateToActiveObjectWorld;

	if(activeEntityId != -1)
		updateWorld();
}

/* Called to see if the translation tool is (still) active. */
bool Tool_Translation::getIsSelected()
{
	return isSelected;
}

/* Called to send updated parameters to the translation tool, if it is still active. */
void Tool_Translation::update(XMVECTOR &rayOrigin, XMVECTOR &rayDir, Camera &theCamera, D3D11_VIEWPORT &theViewport, POINT &mouseCursorPoint)
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

		Vector3 newTranslation;
		
		if(currentlySelectedPlane != camViewTranslationPlane)
		{

			newTranslation.x = newMatrix._41 = originalWorldOfActiveObject._41 + transDelta.m128_f32[0] * scaleFactor;
			newTranslation.y = newMatrix._42 = originalWorldOfActiveObject._42 + transDelta.m128_f32[1] * scaleFactor;
			newTranslation.z = newMatrix._43 = originalWorldOfActiveObject._43 + transDelta.m128_f32[2] * scaleFactor;
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


		Data::Transform* transform = Entity(activeEntityId).fetchData<Data::Transform>();
		transform->toWorldMatrix();
		transform->position = newTranslation;
		originalWorldOfActiveObject = static_cast<XMFLOAT4X4>(world);

		//activeObject->getIRenderable()->setWorld(newMatrix);
	
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
void Tool_Translation::translateObject()
{
	XMVECTOR lastTransDelta = currentlySelectedAxis->getLastTranslationDelta();
	//((Object_Basic*)activeObject)->getIRenderable()->incrementTranslation(lastTransDelta);
}

/* Called when the translation tool is unselected, which makes any hitherto made translation final (and undoable). */
void Tool_Translation::unselect()
{
	// Set the controls' visual and bounding components to the active object's new position and orientation.
	updateWorld();

	if(currentlySelectedPlane)
	{
		currentlySelectedPlane->unselect();
		currentlySelectedPlane = NULL;
	}
	currentlySelectedAxis = NULL;

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
	Vector3 trans = Entity(activeEntityId).fetchData<Data::Transform>()->position;
	Matrix mat_trans;
	mat_trans.CreateTranslation(trans);
	XMMATRIX xm_trans = XMLoadFloat4x4(&mat_trans);

	//XMMATRIX trans = XMMatrixTranslation(activeObject->getIRenderable()->getWorld()._41, activeObject->getIRenderable()->getWorld()._42, activeObject->getIRenderable()->getWorld()._43);

	XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);

	XMFLOAT4X4 world_visual;
	XMStoreFloat4x4(&world_visual, scaling * xm_trans);

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



void Tool_Translation::updateViewPlaneTranslationControlWorld(XMFLOAT3 &camViewVector, XMFLOAT3 &camUpVector)
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
	world_viewPlaneTranslationControl_visual._41 = activeEntityPos.x; //activeObject->getIRenderable()->getWorld().m[3][0];
	world_viewPlaneTranslationControl_visual._42 = activeEntityPos.y; //activeObject->getIRenderable()->getWorld().m[3][1];
	world_viewPlaneTranslationControl_visual._43 = activeEntityPos.z; //activeObject->getIRenderable()->getWorld().m[3][2];
	world_viewPlaneTranslationControl_visual._44 = 1.0f; //activeObject->getIRenderable()->getWorld().m[3][3];
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
	HR(md3dDevice->CreatePixelShader( PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &m_pixelShader));

	HR(D3DCompileFromFile(L"Tool_VS.hlsl", NULL, NULL, "VS", "vs_4_0", NULL, NULL, &VS_Buffer, NULL));
	HR(md3dDevice->CreateVertexShader( VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &m_vertexShader));

	//md3dImmediateContext->PSSetShader(m_pixelShader, 0, 0);
	//md3dImmediateContext->VSSetShader(m_vertexShader, 0, 0);

	D3D11_INPUT_ELEMENT_DESC inputElementDesc [] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},	
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};


	HR(md3dDevice->CreateInputLayout(inputElementDesc, 2, VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), &m_inputLayout));


	ReleaseCOM(VS_Buffer);
	ReleaseCOM(PS_Buffer);

	//md3dImmediateContext->IASetInputLayout(m_inputLayout);

	D3D11_BUFFER_DESC WVP_Desc;
	memset(&WVP_Desc, 0, sizeof(WVP_Desc));
	WVP_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	WVP_Desc.ByteWidth = 16 * 4;//sizeof(float) * 4;
	WVP_Desc.StructureByteStride = 0;//sizeof(float);
	WVP_Desc.Usage = D3D11_USAGE_DEFAULT;

	ConstantBuffer2 WVPinit;
	D3D11_SUBRESOURCE_DATA WVP_Data;
	memset(&WVP_Data, 0, sizeof(WVP_Data));
	WVP_Data.pSysMem = &WVPinit;

	HR(md3dDevice->CreateBuffer(&WVP_Desc, &WVP_Data, &m_WVPBuffer));

	md3dImmediateContext->VSSetConstantBuffers(0, 1, &m_WVPBuffer);

	// *********

	// Create test mesh for visual translation control.
	
	D3D11_BUFFER_DESC vbd;
	D3D11_SUBRESOURCE_DATA vinitData;

	std::vector<Vertex::PosCol> vertices;

	Vertex::PosCol posCol;

	// x axis
	posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
	posCol.Col.x = 1.0f; posCol.Col.y = 0.0f; posCol.Col.z = 1.0f;
	vertices.push_back(posCol);
	posCol.Pos.x = 1.0f; posCol.Col.y = 0.0f; posCol.Col.z = 0.0f;
	posCol.Col.x = 1.0f; posCol.Col.y = 0.0f; posCol.Col.z = 1.0f;
	vertices.push_back(posCol);

	// y axis
	posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
	posCol.Col.x = 1.0f; posCol.Col.y = 1.0f; posCol.Col.z = 0.0f;
	vertices.push_back(posCol);
	posCol.Pos.x = 0.0f; posCol.Pos.y = 1.0f; posCol.Pos.z = 0.0f;
	posCol.Col.x = 1.0f; posCol.Col.y = 1.0f; posCol.Col.z = 0.0f;
	vertices.push_back(posCol);

	// z axis
	posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 0.0f;
	posCol.Col.x = 0.0f; posCol.Col.y = 1.0f; posCol.Col.z = 1.0f;
	vertices.push_back(posCol);
	posCol.Pos.x = 0.0f; posCol.Pos.y = 0.0f; posCol.Pos.z = 1.0f;
	posCol.Col.x = 0.0f; posCol.Col.y = 1.0f; posCol.Col.z = 1.0f;
	vertices.push_back(posCol);

    vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * 6;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshTransToolVB));

	vertices.clear();

	// YZ plane

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

	// ZX plane

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

	// XY plane

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

	// View plane

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

void Tool_Translation::draw(Camera &theCamera, ID3D11DepthStencilView *depthStencilView)
{
	// Draw the translation tool test...

	md3dImmediateContext->PSSetShader(m_pixelShader, 0, 0);
	md3dImmediateContext->VSSetShader(m_vertexShader, 0, 0);

	//md3dImmediateContext->VSSetConstantBuffers(0, 1, )
	//m_constantBuffer->

	//D3D11_MAPPED_SUBRESOURCE mapRes;
	//md3dImmediateContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapRes);
	//XMFLOAT4X4 mappedWVP = (XMFLOAT4X4)mapRes;

	md3dImmediateContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(m_inputLayout);
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	UINT stride = sizeof(Vertex::PosCol);
    UINT offset = 0;

	//D3DX11_TECHNIQUE_DESC techDesc;

	//ID3DX11EffectTechnique *activeMeshTech = Effects::ToolFX->ToolTech;

	XMMATRIX worldViewProj;

	//activeMeshTech->GetDesc( &techDesc );
	//for(UINT p = 0; p < techDesc.Passes; ++p)
 //   {
	//	// Draw the Mesh.
	//	if( GetAsyncKeyState('1') & 0x8000 )
	//		md3dImmediateContext->RSSetState(RenderStates::WireframeRS);
	//	
	//	//md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshRotTool_circle_VB, &stride, &offset);
	//	//md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransToolVB, &stride, &offset);

	//	XMMATRIX identity = XMMatrixIdentity();
	//	XMFLOAT4X4 toolWorld = getWorld_visual();
	//	XMMATRIX world = XMLoadFloat4x4(&toolWorld);
	//	
	//	//float scale = theTranslationTool->getScale();
	//	//world._11 = scale;
	//	//world._22 = scale;
	//	//world._33 = scale;

	//	//XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);

	//	//world._41 = mMeshWorld._41;
	//	//world._42 = mMeshWorld._42;
	//	//world._43 = mMeshWorld._43;

	//	worldViewProj = world * theCamera.View() * theCamera.Proj();

	//	Effects::ToolFX->SetWorldViewProj(worldViewProj);
	//	// Effects::ToolFX->SetMaterial(mMeshMat);

	//	activeMeshTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
	//	md3dImmediateContext->Draw(6, 0);

	//	// Restore default
	//	md3dImmediateContext->RSSetState(0);
	//}

		md3dImmediateContext->IASetInputLayout(m_inputLayout);
	    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

		//mapRes = worldViewProj;
		//Effects::ToolFX->SetWorldViewProj(worldViewProj);


		// Draw control circles.

		Entity e(activeEntityId);

		XMVECTOR rotQuat = e.fetchData<Data::Transform>()->rotation;;
		//XMVECTOR rotQuat = activeObject->getIRenderable()->getRotation();
		XMMATRIX rotation = XMMatrixRotationQuaternion(rotQuat);

		XMFLOAT4X4 toolWorld = getWorld_visual();
		XMMATRIX world = XMLoadFloat4x4(&toolWorld);
		Matrix world2;
		Matrix worldInverted;
		world2.Invert(worldInverted);

		XMMATRIX worldInvTrans = worldInverted.Transpose();
		//XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = rotation * worldInvTrans * theCamera.View() * theCamera.Proj();

		ConstantBuffer2 WVP;
		WVP.WVP = worldViewProj;
		md3dImmediateContext->UpdateSubresource(m_WVPBuffer, 0, NULL, &WVP, 0, 0);

		//Effects::ToolFX->SetWorldViewProj(worldViewProj);

		//md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshRotTool_Xcircle_VB, &stride, &offset);
		//activeMeshTech->GetPassByIndex(0)->Apply(0, md3dImmediateContext);
		//md3dImmediateContext->Draw(65, 0);

		//md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshRotTool_Ycircle_VB, &stride, &offset);
		//activeMeshTech->GetPassByIndex(0)->Apply(0, md3dImmediateContext);
		//md3dImmediateContext->Draw(65, 0);

		//md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshRotTool_Zcircle_VB, &stride, &offset);
		//activeMeshTech->GetPassByIndex(0)->Apply(0, md3dImmediateContext);
		//md3dImmediateContext->Draw(65, 0);

			// Draw control frames.
			md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_yzPlane_VB, &stride, &offset);
			//activeMeshTech->GetPassByIndex(0)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->Draw(5, 0);

			md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_zxPlane_VB, &stride, &offset);
			//activeMeshTech->GetPassByIndex(0)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->Draw(5, 0);

			md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_xyPlane_VB, &stride, &offset);
			//activeMeshTech->GetPassByIndex(0)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->Draw(5, 0);

				//md3dImmediateContext->OMSetDepthStencilState(RenderStates::GreaterEqualDSS, 0);
				////md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

				//XMMATRIX viewControlWorld = XMLoadFloat4x4(&theTranslationTool->getWorld_viewPlaneTranslationControl_visual());
				//
				//float scale = theTranslationTool->getScale();
				//XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);
				//
				//worldViewProj = viewControlWorld*view*proj;
				//Effects::ToolFX->SetWorldViewProj(worldViewProj);

				//md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_viewPlane_VB, &stride, &offset);
				//activeMeshTech->GetPassByIndex(0)->Apply(0, md3dImmediateContext);
				//md3dImmediateContext->Draw(5, 0);

		md3dImmediateContext->OMSetDepthStencilState(0, 0);
}
