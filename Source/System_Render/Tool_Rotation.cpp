#include "stdafx.h"
#include "Tool_Rotation.h"

#include <Core/Data.h>
#include <Core/DataMapper.h>
#include <Core/Events.h>

#include <Core/Command_RotateSceneEntity.h>

Tool_Rotation::Tool_Rotation(/*HWND windowHandle*/)
{
	isSelected = false;
	currentlySelectedHandle = NULL;

	scale = 1.0f;
	XMVECTOR center = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	omniRotateSphereHandle = new Handle_RotationSphere(center, scale /*, windowHandle*/);

	relateToActiveObjectWorld = false;

	activeEntityId = -1;
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
bool Tool_Rotation::tryForSelection(MyRectangle &selectionRectangle, XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, POINT &mouseCursorPoint)
{
	bool aRotationToolHandleWasSelected = false;

	bool rayIntersectsWithToolBoundingBox = false;

	// Check if the picking ray intersects with any of the translation tool's bounding box.
	// ...
	// TEST: Pretend we hit it.
	rayIntersectsWithToolBoundingBox = true;
	
	if(activeEntityId != -1) // Necessary add for the multi-trans functionality. Previously, updateWorld was repeatedly call during hack-y resettings of the active object, which is now only set once, with selection events.
		updateWorld();

	if(rayIntersectsWithToolBoundingBox)
	{
		// Check if the ray intersects with any of the control handles.
		
		float distanceToPointOfIntersection;
		
		// Check if the ray intersects with the omni-rotation sphere.
		bool sphereSelected = omniRotateSphereHandle->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, distanceToPointOfIntersection);
		if(sphereSelected)
		{
			currentlySelectedHandle = omniRotateSphereHandle;
			aRotationToolHandleWasSelected = true;
		}
	}

	if(currentlySelectedHandle && currentlySelectedHandle == omniRotateSphereHandle)
	{
		// Set the cursor icon to the one indicating that the free rotation sphere is being handled.
		SEND_EVENT(&Event_SetCursor(Event_SetCursor::CursorShape::ClosedHandCursor));
	}

	isSelected = aRotationToolHandleWasSelected;
	return aRotationToolHandleWasSelected;
}

/* Called to see if the mouse cursor is hovering over the tool, and what part of it, if any, and sets the cursor accordingly. */
void Tool_Rotation::tryForHover(MyRectangle &selectionRectangle, XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView)
{
	bool aRotationToolHandleWasSelected = false;

	bool rayIntersectsWithToolBoundingBox = false;

	// Check if the picking ray intersects with any of the translation tool's bounding box.
	// ...
	// TEST: Pretend we hit it.
	rayIntersectsWithToolBoundingBox = true;

	IHandle *hoveredHandle = NULL;
	
	if(rayIntersectsWithToolBoundingBox)
	{
		// Check if the ray intersects with any of the control handles.
		
		float distanceToPointOfIntersection;
		
		// Check if the ray intersects with the omni-rotation sphere.
		bool sphereSelected = omniRotateSphereHandle->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, distanceToPointOfIntersection);
		if(sphereSelected)
		{
			hoveredHandle = omniRotateSphereHandle;
		}
	}

	if(hoveredHandle != NULL)
	{
		if(hoveredHandle == omniRotateSphereHandle)
		{
			// Set the cursor icon to the one indicating that the free rotation sphere is being handled.
			SEND_EVENT(&Event_SetCursor(Event_SetCursor::CursorShape::OpenHandCursor));
		}
	}
	else
	{
		SEND_EVENT(&Event_SetCursor(Event_SetCursor::CursorShape::NormalCursor));
	}
}

/* Called to bind the translatable object to the tool, so its translation can be modified. */
void Tool_Rotation::setActiveObject(int entityId)
{
	DataMapper<Data::Selected> map_selected;
	Entity* e;

	originalRotationQuatsOfActiveObject.clear();

	bool thereIsAtLeastOneSelectedEntity = map_selected.hasNext();
	while(map_selected.hasNext())
	{
		e = map_selected.nextEntity();
		Data::Selected* d_selected = e->fetchData<Data::Selected>();

		XMVECTOR rotQuat = e->fetchData<Data::Transform>()->rotation;
		XMFLOAT4 origRotQuat;
		XMStoreFloat4(&origRotQuat, rotQuat);
		originalRotationQuatsOfActiveObject.push_back(origRotQuat);
	 }

	if(thereIsAtLeastOneSelectedEntity && Data::Selected::lastSelected.isValid())
	{
		this->activeEntityId = Data::Selected::lastSelected->toPointer()->id();

		// Set the visual and bounding components of the translation tool to the pivot point of the active object.
		updateWorld();
	}
	else
		activeEntityId = -1;

	omniRotateSphereHandle->resetRotationQuaternion();

	//---

	//this->activeEntityId = entityId;

	//// Set the visual and bounding components of the translation tool to the pivot point of the active object.
	//updateWorld();

	//XMMATRIX world = Entity(activeEntityId).fetchData<Data::Transform>()->toWorldMatrix();
	//XMStoreFloat4x4(&originalWorldOfActiveObject, world);

	//XMVECTOR rotQuat = Entity(activeEntityId).fetchData<Data::Transform>()->rotation;
	//XMStoreFloat4(&originalRotationQuatOfActiveObject, rotQuat);

	/*omniRotateSphereHandle->resetRotationQuaternion();*/
}

/* Called to set the entity at whose pivot the tool is to be displayed, when a selection of one or more entities has been made. */
void Tool_Rotation::setEntityAtWhosePivotTheToolIsToBeDisplayed(int entityId)
{
}

void Tool_Rotation::updateWorld()
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
		
		newWorld._41 = trans->position.x; //objectWorld._41;
		newWorld._42 = trans->position.y; //objectWorld._42;
		newWorld._43 = trans->position.z; //objectWorld._43;

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
		//// Just get the position of the active object, but keep the default orientation.
		//XMMATRIX newWorld = XMMatrixIdentity();
		//XMFLOAT4X4 objectWorld;
		//XMStoreFloat4x4(&objectWorld, activeObject->getIRenderable()->getWorld());
		//newWorld._41 = objectWorld._41;
		//newWorld._42 = objectWorld._42;
		//newWorld._43 = objectWorld._43;

		//// Update the rotation tool's (distance-from-camera-adjusted) scale.
		////newWorld._11 = scale;
		////newWorld._22 = scale;
		////newWorld._33 = scale;
		//XMStoreFloat4x4(&world, newWorld);

		//XMMATRIX logicalWorld = XMLoadFloat4x4(&getWorld_logical());

		//// Transform all the controls.
		//omniRotateSphereHandle->setWorld(logicalWorld);
	}
}

/* Transform all controls to the local coord. sys. of the active object. */
void Tool_Rotation::setRelateToActiveObjectWorld(bool relateToActiveObjectWorld)
{
	this->relateToActiveObjectWorld = relateToActiveObjectWorld;

	if(activeEntityId != -1)
		updateWorld();
}

/* Called to see if the translation tool is (still) active. */
bool Tool_Rotation::getIsSelected()
{
	return isSelected;
}

/* Called to send updated parameters to the translation tool, if it is still active. */
void Tool_Rotation::update(MyRectangle &selectionRectangle, XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, XMMATRIX &camProj, D3D11_VIEWPORT &theViewport, POINT &mouseCursorPoint)
{
	XMVECTOR rotQuaternion = XMVectorSet(0, 0, 0, 1);
	if(currentlySelectedHandle)
	{
		// Pick against the plane to update the translation delta.
		if(currentlySelectedHandle = omniRotateSphereHandle)
		{
			omniRotateSphereHandle->pickSphere(selectionRectangle, rayOrigin, rayDir, camView, camProj, theViewport, mouseCursorPoint);

			rotQuaternion = omniRotateSphereHandle->getTotalRotationQuaternion(); //getLastRotationQuaternion();
			
			//XMVECTOR newRotQuat = XMQuaternionMultiply(XMLoadFloat4(&originalRotationQuatOfActiveObject), rotQuaternion);
			//Data::Transform* transform = Entity(activeEntityId).fetchData<Data::Transform>();
			//transform->rotation = newRotQuat;
		}
	}

	DataMapper<Data::Selected> map_selected;
	Entity* e;

	int i = 0;
	while(map_selected.hasNext())
	{
		e = map_selected.nextEntity();

		XMVECTOR testPivot = XMVectorSet(-4.0f, 0.0f, 0.0f, 1.0f);
		

		XMMATRIX testQuatRotMatrix = XMMatrixRotationQuaternion(rotQuaternion);

		XMMATRIX testPivotTrans = XMMatrixTranslation(testPivot.m128_f32[0], testPivot.m128_f32[1], testPivot.m128_f32[2]);
		XMMATRIX testPivotTransInv = XMMatrixInverse(&XMMatrixDeterminant(testPivotTrans), testPivotTrans);

		XMMATRIX testRotAroundPivotMatrix = testPivotTransInv * testQuatRotMatrix * testPivotTrans;

		XMVECTOR scale, rotation, translation;
		XMMatrixDecompose(&scale, &rotation, &translation, testRotAroundPivotMatrix);

		e->fetchData<Data::Transform>()->rotation = XMQuaternionMultiply(XMLoadFloat4(&originalRotationQuatsOfActiveObject.at(i)), rotation);
		e->fetchData<Data::Transform>()->position = translation;

		++i;
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
	//updateWorld();

	currentlySelectedHandle = NULL;

	std::vector<Command*> rotationCommands;
	DataMapper<Data::Selected> map_selected;
	Entity* e;
	unsigned int i = 0;
	while(map_selected.hasNext())
	{
		e = map_selected.nextEntity();

		Data::Transform* trans = e->fetchData<Data::Transform>();
		Command_RotateSceneEntity *command = new Command_RotateSceneEntity(e->id());
		command->setDoRotQuat(trans->rotation.x, trans->rotation.y, trans->rotation.z, trans->rotation.w);
		command->setUndoRotQuat(originalRotationQuatsOfActiveObject.at(i).x, originalRotationQuatsOfActiveObject.at(i).y, originalRotationQuatsOfActiveObject.at(i).z, originalRotationQuatsOfActiveObject.at(i).w);
		rotationCommands.push_back(command);
		
		//SEND_EVENT(&Event_StoreCommandInCommandHistory(command, false));
		
		++i;
	}

	SEND_EVENT(&Event_StoreCommandsAsSingleEntryInCommandHistoryGUI(&rotationCommands, false));

	setActiveObject(1);

	// Set the cursor icon to the default/scene cursor icon.
	SEND_EVENT(&Event_SetCursor(Event_SetCursor::CursorShape::NormalCursor));

	isSelected = false;
}

XMFLOAT4X4 Tool_Rotation::getWorld_logical()
{
	XMVECTOR testPivot = XMVectorSet(-4.0f, 0.0f, 0.0f, 1.0f);

	XMVECTOR trans = Vector3(testPivot); //Entity(activeEntityId).fetchData<Data::Transform>()->position
	XMMATRIX translation = XMMatrixTranslationFromVector(trans);

	XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);

	XMFLOAT4X4 world;
	XMStoreFloat4x4(&world, scaling * translation);

	return world;
}

XMFLOAT4X4 Tool_Rotation::getWorld_visual()
{

	XMVECTOR testPivot = XMVectorSet(-4.0f, 0.0f, 0.0f, 1.0f);

	XMVECTOR trans = Vector3(testPivot); //Entity(activeEntityId).fetchData<Data::Transform>()->position
	XMMATRIX translation = XMMatrixTranslationFromVector(trans);

	XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);

	XMFLOAT4X4 world_visual;
	XMStoreFloat4x4(&world_visual, scaling * translation);

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

	Vector3 activeEntityPos = Entity(activeEntityId).fetchData<Data::Transform>()->position;
	world_viewPlaneTranslationControl_visual._41 = activeEntityPos.x;
	world_viewPlaneTranslationControl_visual._42 = activeEntityPos.y;
	world_viewPlaneTranslationControl_visual._43 = activeEntityPos.z;
	world_viewPlaneTranslationControl_visual._44 = 1.0f;
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

int Tool_Rotation::getActiveObject()
{
	return activeEntityId;
}

void Tool_Rotation::init(ID3D11Device *device, ID3D11DeviceContext *deviceContext)
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

	// Create mesh for visual rotation control.

	GeometryGenerator geoGen;

	GeometryGenerator::MeshData2 circleMeshData;
	GeometryGenerator::MeshData2 circleMeshData2;
	GeometryGenerator::MeshData2 circleMeshData3;
	GeometryGenerator::MeshData2 circleMeshData4;

	XMVECTOR color = XMVectorSet(1.0f, 0.0f, 1.0f, 1.0f);
	XMVECTOR center = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	float radius = 1.0f;
	int nrOfPoints = 64;
	geoGen.createLineListCircle(center, radius, nrOfPoints, color, circleMeshData, 'z');

	D3D11_BUFFER_DESC vbd;
	D3D11_SUBRESOURCE_DATA vinitData;

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * circleMeshData.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vinitData.pSysMem = &circleMeshData.Vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshRotTool_circle_VB));

	//

	color = XMVectorSet(1.0f, 0.0f, 1.0f, 1.0f);
	center = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	radius = 1.0f;
	nrOfPoints = 64;
	geoGen.createLineListCircle(center, radius, nrOfPoints, color, circleMeshData2, 'x');

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * circleMeshData2.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vinitData.pSysMem = &circleMeshData2.Vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshRotTool_Xcircle_VB));

	color = XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f);
	center = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	radius = 1.0f;
	nrOfPoints = 64;
	geoGen.createLineListCircle(center, radius, nrOfPoints, color, circleMeshData3, 'y');

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * circleMeshData3.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vinitData.pSysMem = &circleMeshData3.Vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshRotTool_Ycircle_VB));

	color = XMVectorSet(0.0f, 1.0f, 1.0f, 1.0f);
	center = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	radius = 1.0f;
	nrOfPoints = 64;
	geoGen.createLineListCircle(center, radius, nrOfPoints, color, circleMeshData4, 'z');

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * circleMeshData4.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vinitData.pSysMem = &circleMeshData4.Vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshRotTool_Zcircle_VB));
}

void Tool_Rotation::draw(XMMATRIX &camView, XMMATRIX &camProj, ID3D11DepthStencilView *depthStencilView)
{
	// Draw the rotation tool...

	md3dImmediateContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	md3dImmediateContext->PSSetShader(m_pixelShader, 0, 0);
	md3dImmediateContext->VSSetShader(m_vertexShader, 0, 0);

	md3dImmediateContext->IASetInputLayout(m_inputLayout);
   
	UINT stride = sizeof(Vertex::PosCol);
    UINT offset = 0;
	
	Entity e(activeEntityId);

	XMVECTOR rotQuat = e.fetchData<Data::Transform>()->rotation;
	XMMATRIX rotation = XMMatrixRotationQuaternion(rotQuat);

	XMFLOAT4X4 toolWorld = getWorld_visual();
	XMMATRIX world = XMLoadFloat4x4(&toolWorld);
	
	XMMATRIX worldViewProj = rotation * world * camView * camProj; // Semi-HACK w/ the rotation, or rather here it is hardcoded that it will rotate about with the objects rotation.
	worldViewProj = XMMatrixTranspose(worldViewProj);

	ConstantBuffer2 WVP;
	WVP.WVP = worldViewProj;
	md3dImmediateContext->UpdateSubresource(m_WVPBuffer, 0, NULL, &WVP, 0, 0);
	md3dImmediateContext->VSSetConstantBuffers(0, 1, &m_WVPBuffer);

	// Draw control circles.

	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshRotTool_Xcircle_VB, &stride, &offset);
	md3dImmediateContext->Draw(65, 0);

	md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshRotTool_Ycircle_VB, &stride, &offset);
	md3dImmediateContext->Draw(65, 0);

	md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshRotTool_Zcircle_VB, &stride, &offset);
	md3dImmediateContext->Draw(65, 0);
	
	//md3dImmediateContext->OMSetDepthStencilState(0, 0); // Perhaps unnecessary.
}

