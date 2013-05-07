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

	//md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

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

	//md3dImmediateContext->OMSetBlendState(blendState, NULL, NULL);

	//ReleaseCOM(blendState);

	//md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_xyTriangleListRectangle_VB, &stride, &offset);
	//md3dImmediateContext->Draw(6, 0);

	//md3dImmediateContext->OMSetBlendState(NULL, NULL, NULL);

	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_yzPlane_VB, &stride, &offset);
	md3dImmediateContext->Draw(5, 0);

	md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_zxPlane_VB, &stride, &offset);
	md3dImmediateContext->Draw(5, 0);

	md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_xyPlane_VB, &stride, &offset);
	md3dImmediateContext->Draw(5, 0);

	md3dImmediateContext->OMSetDepthStencilState(RenderStates::GreaterEqualDSS, 0);
	md3dImmediateContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	XMMATRIX viewControlWorld = XMLoadFloat4x4(&getWorld_viewPlaneTranslationControl_visual());

	float scale = getScale();
	XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);

	worldViewProj = viewControlWorld * camView * camProj;
	worldViewProj = XMMatrixTranspose(worldViewProj);

	WVP.WVP = worldViewProj;
	md3dImmediateContext->UpdateSubresource(m_WVPBuffer, 0, NULL, &WVP, 0, 0);
	md3dImmediateContext->VSSetConstantBuffers(0, 1, &m_WVPBuffer);

	md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshTransTool_viewPlane_VB, &stride, &offset);
	md3dImmediateContext->Draw(5, 0);

	//md3dImmediateContext->OMSetDepthStencilState(0, 0); // Perhaps unnecessary:
}