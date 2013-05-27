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

	scale = 1.1f;
	XMVECTOR center = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	omniRotateSphereHandle = new Handle_RotationSphere(center, scale /*, windowHandle*/);

	relateToActiveObjectWorld = false;
}

Tool_Rotation::~Tool_Rotation()
{
	delete omniRotateSphereHandle;

	delete xRotationHandle;
	delete yRotationHandle;
	delete zRotationHandle;
	delete viewAxisRotationHandle;
}

void Tool_Rotation::setIsVisible(bool &isVisible)
{
	this->isVisible = isVisible;
}

/* Called for an instance of picking, possibly resulting in the tool being selected. */
bool Tool_Rotation::tryForSelection(MyRectangle &selectionRectangle, XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, XMMATRIX &camProj, POINT &mouseCursorPoint)
{
	bool aRotationToolHandleWasSelected = false;

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
		
		bool aSingleAxisRotationHandleWasSelected = false;
		bool sphereSelected = false;

		// Check if any of the control circles are selected.

		XMVECTOR camLookAtVector = -XMVector3Normalize(SETTINGS()->entity_camera->fetchData<Data::Camera>()->getLookVector());
		
		if(!aSingleAxisRotationHandleWasSelected)
		{
			xRotationHandle->setSelectionBlockingPlaneNormal(camLookAtVector);
			aSingleAxisRotationHandleWasSelected = xRotationHandle->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, camProj, distanceToPointOfIntersection);
			if(aSingleAxisRotationHandleWasSelected)
			{
				currentlySelectedHandle = omniRotateSphereHandle; //xRotationHandle;
				omniRotateSphereHandle->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, distanceToPointOfIntersection);

				XMVECTOR rotQuat = activeEntity->fetchData<Data::Transform>()->rotation;
				XMMATRIX rotation = XMMatrixRotationQuaternion(rotQuat);

				XMVECTOR xAxis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
				xAxis = XMVector3Transform(xAxis, rotation);
				xAxis = XMVector3Normalize(xAxis);

				omniRotateSphereHandle->constrainRotationToOneFixedAxis(true, xAxis);
				aRotationToolHandleWasSelected = true;
			}
		}
		
		if(!aSingleAxisRotationHandleWasSelected)
		{
			yRotationHandle->setSelectionBlockingPlaneNormal(camLookAtVector);
			aSingleAxisRotationHandleWasSelected = yRotationHandle->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, camProj, distanceToPointOfIntersection);
			if(aSingleAxisRotationHandleWasSelected)
			{
				currentlySelectedHandle = omniRotateSphereHandle; //yRotationHandle;

				XMVECTOR rotQuat = activeEntity->fetchData<Data::Transform>()->rotation;
				XMMATRIX rotation = XMMatrixRotationQuaternion(rotQuat);

				XMVECTOR yAxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
				yAxis = XMVector3Transform(yAxis, rotation);
				yAxis = XMVector3Normalize(yAxis);

				omniRotateSphereHandle->constrainRotationToOneFixedAxis(true, yAxis);
				aRotationToolHandleWasSelected = true;
			}
		}
		
		if(!aSingleAxisRotationHandleWasSelected)
		{
			zRotationHandle->setSelectionBlockingPlaneNormal(camLookAtVector);
			aSingleAxisRotationHandleWasSelected = zRotationHandle->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, camProj, distanceToPointOfIntersection);
			if(aSingleAxisRotationHandleWasSelected)
			{
				currentlySelectedHandle = omniRotateSphereHandle; //zRotationHandle;

				XMVECTOR rotQuat = activeEntity->fetchData<Data::Transform>()->rotation;
				XMMATRIX rotation = XMMatrixRotationQuaternion(rotQuat);

				XMVECTOR zAxis = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
				zAxis = XMVector3Transform(zAxis, rotation);
				zAxis = XMVector3Normalize(zAxis);

				omniRotateSphereHandle->constrainRotationToOneFixedAxis(true, zAxis);
				aRotationToolHandleWasSelected = true;
			}
		}

		if(!aSingleAxisRotationHandleWasSelected)
		{
			viewAxisRotationHandle->setSelectionBlockingPlaneNormal(camLookAtVector);
			aSingleAxisRotationHandleWasSelected = viewAxisRotationHandle->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, camProj, distanceToPointOfIntersection);
			if(aSingleAxisRotationHandleWasSelected)
			{
				currentlySelectedHandle = omniRotateSphereHandle;
				
				XMVECTOR rotQuat = activeEntity->fetchData<Data::Transform>()->rotation;
				XMMATRIX rotation = XMMatrixRotationQuaternion(rotQuat);

				XMVECTOR viewVectorAxis = -SETTINGS()->entity_camera->fetchData<Data::Camera>()->getLookVector();	// TO-DO:	Get camera's view vector and use it here.
				//viewVectorAxis = XMVector3Transform(viewVectorAxis, rotation);
				viewVectorAxis = XMVector3Normalize(viewVectorAxis);

				omniRotateSphereHandle->constrainRotationToOneFixedAxis(true, viewVectorAxis);
				aRotationToolHandleWasSelected = true;
			}
		}

		if(!aSingleAxisRotationHandleWasSelected)
		{
			// Check if the ray intersects with the omni-rotation sphere.
			sphereSelected = omniRotateSphereHandle->tryForSelection(selectionRectangle, rayOrigin, rayDir, camView, distanceToPointOfIntersection);
			if(sphereSelected)
			{
				XMVECTOR dummy = XMVectorZero();
				omniRotateSphereHandle->constrainRotationToOneFixedAxis(false, dummy);

				currentlySelectedHandle = omniRotateSphereHandle;
				aRotationToolHandleWasSelected = true;
			}
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
			//SEND_EVENT(&Event_SetCursor(Event_SetCursor::CursorShape::OpenHandCursor));
		}
	}
	else
	{
		SEND_EVENT(&Event_SetCursor(Event_SetCursor::CursorShape::SceneCursor));
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
		this->activeEntity = Data::Selected::lastSelected->toPointer();

		// Set the visual and bounding components of the translation tool to the pivot point of the active object.
		updateWorld();
	}
	else
	{
		activeEntity.invalidate();
	}

	omniRotateSphereHandle->resetRotationQuaternion();

	//---

	//this->activeEntityId = entityId;

	//// Set the visual and bounding components of the translation tool to the pivot point of the active object.
	//updateWorld();

	//XMMATRIX world = activeEntity->fetchData<Data::Transform>()->toWorldMatrix();
	//XMStoreFloat4x4(&originalWorldOfActiveObject, world);

	//XMVECTOR rotQuat = activeEntity->fetchData<Data::Transform>()->rotation;
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

		Data::Transform* trans = activeEntity->fetchData<Data::Transform>();
		objectWorld = static_cast<XMFLOAT4X4>(world);
		
		newWorld._41 = trans->position.x;
		newWorld._42 = trans->position.y;
		newWorld._43 = trans->position.z;

		XMStoreFloat4x4(&world, newWorld);

		XMMATRIX logicalWorld = XMLoadFloat4x4(&getWorld_logical());

		// Transform all the controls.
		omniRotateSphereHandle->setWorld(logicalWorld);

		logicalWorld = XMLoadFloat4x4(&getWorldRotationCircles_logical());
		xRotationHandle->setWorld(logicalWorld);
		yRotationHandle->setWorld(logicalWorld);
		zRotationHandle->setWorld(logicalWorld);
		viewAxisRotationHandle->setWorld(XMLoadFloat4x4(&getWorld_viewRectangle_logical()));
	}
	else
	{
	}
}

/* Transform all controls to the local coord. sys. of the active object. */
void Tool_Rotation::setRelateToActiveObjectWorld(bool relateToActiveObjectWorld)
{
	this->relateToActiveObjectWorld = relateToActiveObjectWorld;

	if(activeEntity.isValid())
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
		if(currentlySelectedHandle == omniRotateSphereHandle)
		{
			omniRotateSphereHandle->pickSphere(selectionRectangle, rayOrigin, rayDir, camView, camProj, theViewport, mouseCursorPoint);

			rotQuaternion = omniRotateSphereHandle->getTotalRotationQuaternion(); //getLastRotationQuaternion();

			//rotQuaternion.m128_f32[1] = 0.0f;
			//rotQuaternion.m128_f32[2] = 0.0f;
			
			//XMVECTOR newRotQuat = XMQuaternionMultiply(XMLoadFloat4(&originalRotationQuatOfActiveObject), rotQuaternion);
			//Data::Transform* transform = activeEntity->fetchData<Data::Transform>();
			//transform->rotation = newRotQuat;
		}
		else if(currentlySelectedHandle == zRotationHandle)
		{
			zRotationHandle->pickAxisPlane(rayOrigin, rayDir, camView);

			rotQuaternion = zRotationHandle->getCurrentRotationQuaternion();
		}
	}


	DataMapper<Data::Selected> map_selected;
	Entity* e;

	int i = 0;
	while(map_selected.hasNext())
	{
		e = map_selected.nextEntity();

		//XMVECTOR testPivot = XMVectorSet(-4.0f, 0.0f, 0.0f, 1.0f);
		//
		//XMMATRIX testQuatRotMatrix = XMMatrixRotationQuaternion(rotQuaternion);

		//XMMATRIX testPivotTrans = XMMatrixTranslation(testPivot.m128_f32[0], testPivot.m128_f32[1], testPivot.m128_f32[2]);
		//XMMATRIX testPivotTransInv = XMMatrixInverse(&XMMatrixDeterminant(testPivotTrans), testPivotTrans);

		//XMMATRIX testRotAroundPivotMatrix = testPivotTransInv * testQuatRotMatrix * testPivotTrans;

		//XMVECTOR scale, rotation, translation;
		//XMMatrixDecompose(&scale, &rotation, &translation, testRotAroundPivotMatrix);

		e->fetchData<Data::Transform>()->rotation = XMQuaternionMultiply(XMLoadFloat4(&originalRotationQuatsOfActiveObject.at(i)), rotQuaternion);
		//e->fetchData<Data::Transform>()->position = translation;

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

	xRotationHandle->unselect();
	yRotationHandle->unselect();
	zRotationHandle->unselect();
	viewAxisRotationHandle->unselect();
	omniRotateSphereHandle->unselect();

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
	SEND_EVENT(&Event_SetCursor(Event_SetCursor::CursorShape::SceneCursor));

	isSelected = false;
}

XMFLOAT4X4 Tool_Rotation::getWorld_logical()
{
	//XMVECTOR testPivot = XMVectorSet(-4.0f, 0.0f, 0.0f, 1.0f);
	
	XMVECTOR trans = activeEntity->fetchData<Data::Transform>()->position; //Vector3(testPivot);
	XMMATRIX translation = XMMatrixTranslationFromVector(trans);

	XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);

	XMVECTOR rotQuat = activeEntity->fetchData<Data::Transform>()->rotation;
	XMMATRIX rotation = XMMatrixRotationQuaternion(rotQuat);

	XMFLOAT4X4 world;
	XMStoreFloat4x4(&world, scaling * translation);

	return world;
}

XMFLOAT4X4 Tool_Rotation::getWorldRotationCircles_logical()
{
	//XMVECTOR testPivot = XMVectorSet(-4.0f, 0.0f, 0.0f, 1.0f);

	XMVECTOR trans = activeEntity->fetchData<Data::Transform>()->position; //Vector3(testPivot);
	XMMATRIX translation = XMMatrixTranslationFromVector(trans);

	XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);

	XMVECTOR rotQuat = activeEntity->fetchData<Data::Transform>()->rotation;
	XMMATRIX rotation = XMMatrixRotationQuaternion(rotQuat);

	XMFLOAT4X4 world;
	XMStoreFloat4x4(&world, scaling * rotation * translation);

	return world;
}

XMFLOAT4X4 Tool_Rotation::getWorld_visual()
{
	//XMVECTOR testPivot = XMVectorSet(-4.0f, 0.0f, 0.0f, 1.0f);

	XMVECTOR trans = activeEntity->fetchData<Data::Transform>()->position; //Vector3(testPivot);
	XMMATRIX translation = XMMatrixTranslationFromVector(trans);

	XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);

	XMVECTOR rotQuat = activeEntity->fetchData<Data::Transform>()->rotation;
	XMMATRIX rotation = XMMatrixRotationQuaternion(rotQuat);

	XMFLOAT4X4 world_visual;
	XMStoreFloat4x4(&world_visual, scaling * rotation * translation);

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


void Tool_Rotation::updateViewRectangleWorld(XMFLOAT3 &camViewVector, XMFLOAT3 &camUpVector, XMFLOAT3 &camRightVector)
{
	XMVECTOR loadedCamViewVector = XMLoadFloat3(&camViewVector);
	XMVECTOR loadedCamUpVector = XMLoadFloat3(&camUpVector);
	XMVECTOR loadedCamRightVector = XMLoadFloat3(&camRightVector); //XMVector3Cross(loadedCamUpVector, loadedCamViewVector);

	XMStoreFloat3((XMFLOAT3*)world_viewRectangle_logical.m[0], loadedCamRightVector);
	world_viewRectangle_logical.m[0][3] = 0.0f;
	XMStoreFloat3((XMFLOAT3*)world_viewRectangle_logical.m[1], loadedCamUpVector);
	world_viewRectangle_logical.m[1][3] = 0.0f;
	XMStoreFloat3((XMFLOAT3*)world_viewRectangle_logical.m[2], loadedCamViewVector);
	world_viewRectangle_logical.m[2][3] = 0.0f;

	Vector3 activeEntityPos = activeEntity->fetchData<Data::Transform>()->position;
	world_viewRectangle_logical._41 = activeEntityPos.x; //world.m[3][0];
	world_viewRectangle_logical._42 = activeEntityPos.y; //world.m[3][1];
	world_viewRectangle_logical._43 = activeEntityPos.z; //world.m[3][2];
	world_viewRectangle_logical._44 = 1.0f; //world.m[3][3];

	XMStoreFloat3((XMFLOAT3*)world_viewRectangle_visual.m[0], loadedCamRightVector);
	world_viewRectangle_visual.m[0][3] = 0.0f;
	XMStoreFloat3((XMFLOAT3*)world_viewRectangle_visual.m[1], loadedCamUpVector);
	world_viewRectangle_visual.m[1][3] = 0.0f;
	XMStoreFloat3((XMFLOAT3*)world_viewRectangle_visual.m[2], loadedCamViewVector);
	world_viewRectangle_visual.m[2][3] = 0.0f;

	world_viewRectangle_visual._41 = activeEntityPos.x;
	world_viewRectangle_visual._42 = activeEntityPos.y;
	world_viewRectangle_visual._43 = activeEntityPos.z;
	world_viewRectangle_visual._44 = 1.0f;
}

XMFLOAT4X4 Tool_Rotation::getWorld_viewRectangle_logical()
{
	XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);

	XMMATRIX loadedWorld = XMLoadFloat4x4(&world_viewRectangle_logical);
	XMFLOAT4X4 logicalWorld;
	XMStoreFloat4x4(&logicalWorld, scaling * loadedWorld);

	return logicalWorld;
}

XMFLOAT4X4 Tool_Rotation::getWorld_viewRectangle_visual()
{
	XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);

	XMMATRIX loadedWorld = XMLoadFloat4x4(&world_viewRectangle_visual);
	XMFLOAT4X4 visualWorld;
	XMStoreFloat4x4(&visualWorld, scaling * loadedWorld);

	return visualWorld;
}

EntityPointer Tool_Rotation::getActiveObject()
{
	return activeEntity;
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

	D3D11_BUFFER_DESC ColorSchemeId_Desc;
	ZeroMemory(&ColorSchemeId_Desc, sizeof(ColorSchemeId_Desc)); //memset(&WVP_Desc, 0, sizeof(WVP_Desc));
	ColorSchemeId_Desc.Usage = D3D11_USAGE_DEFAULT;
	ColorSchemeId_Desc.ByteWidth = 16;
	ColorSchemeId_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	HR(md3dDevice->CreateBuffer(&ColorSchemeId_Desc, NULL, &m_ColorSchemeIdBuffer));

	// Create mesh for visual rotation control.

	GeometryGenerator geoGen;

	D3D11_BUFFER_DESC vbd;
	D3D11_SUBRESOURCE_DATA vinitData;

	std::vector<XMFLOAT4> listOfLineSegmentsAsPoints;
	listOfLineSegmentsAsPoints.resize(0);

	GeometryGenerator::MeshData2 circleMeshDataView;
	GeometryGenerator::MeshData2 circleMeshDataX;
	GeometryGenerator::MeshData2 circleMeshDataY;
	GeometryGenerator::MeshData2 circleMeshDataZ;

	XMVECTOR xAxis = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR yAxis = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	XMVECTOR zAxis = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);

	XMVECTOR color = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR center = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	float radius = 1.1f;
	int nrOfPoints = 64;
	//geoGen.createLineListCircle(center, radius, nrOfPoints, color, circleMeshDataView, 'z');

	//// Convert line strip to line list.
	//for(unsigned int i = 0; i < circleMeshDataView.Vertices.size(); i = i + 1)
	//{
	//	if(i != circleMeshDataView.Vertices.size() - 1)
	//	{
	//		XMFLOAT4 linePointA = XMFLOAT4(circleMeshDataView.Vertices.at(i).Position.x, circleMeshDataView.Vertices.at(i).Position.y, circleMeshDataView.Vertices.at(i).Position.z, 1.0f);
	//		XMFLOAT4 linePointB = XMFLOAT4(circleMeshDataView.Vertices.at(i + 1).Position.x, circleMeshDataView.Vertices.at(i + 1).Position.y, circleMeshDataView.Vertices.at(i + 1).Position.z, 1.0f);

	//		listOfLineSegmentsAsPoints.push_back(linePointA);
	//		listOfLineSegmentsAsPoints.push_back(linePointB);
	//	}
	//	else
	//	{
	//		XMFLOAT4 linePointA = XMFLOAT4(circleMeshDataView.Vertices.at(i).Position.x, circleMeshDataView.Vertices.at(i).Position.y, circleMeshDataView.Vertices.at(i).Position.z, 1.0f);
	//		XMFLOAT4 linePointB = XMFLOAT4(circleMeshDataView.Vertices.at(0).Position.x, circleMeshDataView.Vertices.at(0).Position.y, circleMeshDataView.Vertices.at(0).Position.z, 1.0f);

	//		listOfLineSegmentsAsPoints.push_back(linePointA);
	//		listOfLineSegmentsAsPoints.push_back(linePointB);
	//	}
	//}

	//viewAxisRotationHandle = new Handle_RotationCircle(zAxis, listOfLineSegmentsAsPoints, 'z');
	//listOfLineSegmentsAsPoints.clear();
	//
	//vbd.Usage = D3D11_USAGE_IMMUTABLE;
	//vbd.ByteWidth = sizeof(Vertex::PosCol) * circleMeshDataView.Vertices.size();
 //   vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
 //   vbd.CPUAccessFlags = 0;
 //   vbd.MiscFlags = 0;
	//vinitData.pSysMem = &circleMeshDataView.Vertices[0];
	//HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshRotTool_circle_VB));

	color = XMVectorSet(1.0f, 0.0f, 1.0f, 1.0f);
	center = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	radius = 1.0f;
	nrOfPoints = 64;
	geoGen.createLineListCircle(center, radius, nrOfPoints, color, circleMeshDataX, 'x');

	// Convert line strip to line list.
	for(unsigned int i = 0; i < circleMeshDataX.Vertices.size(); i = i + 1)
	{
		if(i != circleMeshDataX.Vertices.size() - 1)
		{
			XMFLOAT4 linePointA = XMFLOAT4(circleMeshDataX.Vertices.at(i).Position.x, circleMeshDataX.Vertices.at(i).Position.y, circleMeshDataX.Vertices.at(i).Position.z, 1.0f);
			XMFLOAT4 linePointB = XMFLOAT4(circleMeshDataX.Vertices.at(i + 1).Position.x, circleMeshDataX.Vertices.at(i + 1).Position.y, circleMeshDataX.Vertices.at(i + 1).Position.z, 1.0f);

			listOfLineSegmentsAsPoints.push_back(linePointA);
			listOfLineSegmentsAsPoints.push_back(linePointB);
		}
		else
		{

			XMFLOAT4 linePointA = XMFLOAT4(circleMeshDataX.Vertices.at(i).Position.x, circleMeshDataX.Vertices.at(i).Position.y, circleMeshDataX.Vertices.at(i).Position.z, 1.0f);
			XMFLOAT4 linePointB = XMFLOAT4(circleMeshDataX.Vertices.at(0).Position.x, circleMeshDataX.Vertices.at(0).Position.y, circleMeshDataX.Vertices.at(0).Position.z, 1.0f);

			listOfLineSegmentsAsPoints.push_back(linePointA);
			listOfLineSegmentsAsPoints.push_back(linePointB);
		}
	}

	xRotationHandle = new Handle_RotationCircle(xAxis, listOfLineSegmentsAsPoints, 'x');
	listOfLineSegmentsAsPoints.clear();

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * circleMeshDataX.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vinitData.pSysMem = &circleMeshDataX.Vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshRotTool_Xcircle_VB));

	color = XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f);
	center = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	radius = 1.0f;
	nrOfPoints = 64;
	geoGen.createLineListCircle(center, radius, nrOfPoints, color, circleMeshDataY, 'y');

	// Convert line strip to line list.
	for(unsigned int i = 0; i < circleMeshDataY.Vertices.size(); i = i + 1)
	{
		if(i != circleMeshDataY.Vertices.size() - 1)
		{
			XMFLOAT4 linePointA = XMFLOAT4(circleMeshDataY.Vertices.at(i).Position.x, circleMeshDataY.Vertices.at(i).Position.y, circleMeshDataY.Vertices.at(i).Position.z, 1.0f);
			XMFLOAT4 linePointB = XMFLOAT4(circleMeshDataY.Vertices.at(i + 1).Position.x, circleMeshDataY.Vertices.at(i + 1).Position.y, circleMeshDataY.Vertices.at(i + 1).Position.z, 1.0f);

			listOfLineSegmentsAsPoints.push_back(linePointA);
			listOfLineSegmentsAsPoints.push_back(linePointB);
		}
		else
		{

			XMFLOAT4 linePointA = XMFLOAT4(circleMeshDataY.Vertices.at(i).Position.x, circleMeshDataY.Vertices.at(i).Position.y, circleMeshDataY.Vertices.at(i).Position.z, 1.0f);
			XMFLOAT4 linePointB = XMFLOAT4(circleMeshDataY.Vertices.at(0).Position.x, circleMeshDataY.Vertices.at(0).Position.y, circleMeshDataY.Vertices.at(0).Position.z, 1.0f);

			listOfLineSegmentsAsPoints.push_back(linePointA);
			listOfLineSegmentsAsPoints.push_back(linePointB);
		}
	}

	yRotationHandle = new Handle_RotationCircle(yAxis, listOfLineSegmentsAsPoints, 'y');
	listOfLineSegmentsAsPoints.clear();

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * circleMeshDataY.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vinitData.pSysMem = &circleMeshDataY.Vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshRotTool_Ycircle_VB));

	color = XMVectorSet(0.0f, 1.0f, 1.0f, 1.0f);
	center = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	radius = 1.0f;
	nrOfPoints = 64;
	geoGen.createLineListCircle(center, radius, nrOfPoints, color, circleMeshDataZ, 'z');

	// Convert line strip to line list.
	for(unsigned int i = 0; i < circleMeshDataZ.Vertices.size(); i = i + 1)
	{
		if(i != circleMeshDataZ.Vertices.size() - 1)
		{
			XMFLOAT4 linePointA = XMFLOAT4(circleMeshDataZ.Vertices.at(i).Position.x, circleMeshDataZ.Vertices.at(i).Position.y, circleMeshDataZ.Vertices.at(i).Position.z, 1.0f);
			XMFLOAT4 linePointB = XMFLOAT4(circleMeshDataZ.Vertices.at(i + 1).Position.x, circleMeshDataZ.Vertices.at(i + 1).Position.y, circleMeshDataZ.Vertices.at(i + 1).Position.z, 1.0f);

			listOfLineSegmentsAsPoints.push_back(linePointA);
			listOfLineSegmentsAsPoints.push_back(linePointB);
		}
		else
		{

			XMFLOAT4 linePointA = XMFLOAT4(circleMeshDataZ.Vertices.at(i).Position.x, circleMeshDataZ.Vertices.at(i).Position.y, circleMeshDataZ.Vertices.at(i).Position.z, 1.0f);
			XMFLOAT4 linePointB = XMFLOAT4(circleMeshDataZ.Vertices.at(0).Position.x, circleMeshDataZ.Vertices.at(0).Position.y, circleMeshDataZ.Vertices.at(0).Position.z, 1.0f);

			listOfLineSegmentsAsPoints.push_back(linePointA);
			listOfLineSegmentsAsPoints.push_back(linePointB);
		}
	}

	zRotationHandle = new Handle_RotationCircle(zAxis, listOfLineSegmentsAsPoints, 'z');
	listOfLineSegmentsAsPoints.clear();

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * circleMeshDataZ.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vinitData.pSysMem = &circleMeshDataZ.Vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshRotTool_Zcircle_VB));

	// View circle.
	circleMeshDataZ.Vertices.clear();
	color = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	center = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	radius = 1.15f;
	nrOfPoints = 64;
	geoGen.createLineListCircle(center, radius, nrOfPoints, color, circleMeshDataZ, 'z');

	// Convert line strip to line list.
	for(unsigned int i = 0; i < circleMeshDataZ.Vertices.size(); i = i + 1)
	{
		if(i != circleMeshDataZ.Vertices.size() - 1)
		{
			XMFLOAT4 linePointA = XMFLOAT4(circleMeshDataZ.Vertices.at(i).Position.x, circleMeshDataZ.Vertices.at(i).Position.y, circleMeshDataZ.Vertices.at(i).Position.z, 1.0f);
			XMFLOAT4 linePointB = XMFLOAT4(circleMeshDataZ.Vertices.at(i + 1).Position.x, circleMeshDataZ.Vertices.at(i + 1).Position.y, circleMeshDataZ.Vertices.at(i + 1).Position.z, 1.0f);

			listOfLineSegmentsAsPoints.push_back(linePointA);
			listOfLineSegmentsAsPoints.push_back(linePointB);
		}
		else
		{

			XMFLOAT4 linePointA = XMFLOAT4(circleMeshDataZ.Vertices.at(i).Position.x, circleMeshDataZ.Vertices.at(i).Position.y, circleMeshDataZ.Vertices.at(i).Position.z, 1.0f);
			XMFLOAT4 linePointB = XMFLOAT4(circleMeshDataZ.Vertices.at(0).Position.x, circleMeshDataZ.Vertices.at(0).Position.y, circleMeshDataZ.Vertices.at(0).Position.z, 1.0f);

			listOfLineSegmentsAsPoints.push_back(linePointA);
			listOfLineSegmentsAsPoints.push_back(linePointB);
		}
	}

	viewAxisRotationHandle = new Handle_RotationCircle(zAxis, listOfLineSegmentsAsPoints, 'z');
	listOfLineSegmentsAsPoints.clear();

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * circleMeshDataZ.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vinitData.pSysMem = &circleMeshDataZ.Vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshRotTool_viewCircle_VB));

	// Init guiding lines...

	GeometryGenerator::MeshData2 meshVertices;	float lengthOfLine = 3000.0f;
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
	geoGen.createLine(endPointA, endPointB, 79998, colorA, colorB, localSpaceTrans, meshVertices);

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * meshVertices.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vinitData.pSysMem = &meshVertices.Vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshRotTool_xAxisLine_VB));

	meshVertices.Vertices.clear();

	// Y guiding line.

	localSpaceTrans = XMMatrixRotationZ(-Math::Pi / 2);
	endPointA = XMVectorSet(-lengthOfLine, 0.0f, 0.0f, 1.0f);
	endPointB = XMVectorSet(+lengthOfLine, 0.0f, 0.0f, 1.0f);
	colorA = XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f);
	colorB = XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f);
	geoGen.createLine(endPointA, endPointB, 79998, colorA, colorB, localSpaceTrans, meshVertices);

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * meshVertices.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vinitData.pSysMem = &meshVertices.Vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshRotTool_yAxisLine_VB));

	meshVertices.Vertices.clear();

	// Z guiding line.

	localSpaceTrans = XMMatrixRotationY(Math::Pi / 2);
	endPointA = XMVectorSet(-lengthOfLine, 0.0f, 0.0f, 1.0f);
	endPointB = XMVectorSet(+lengthOfLine, 0.0f, 0.0f, 1.0f);
	colorA = XMVectorSet(0.0f, 1.0f, 1.0f, 1.0f);
	colorB = XMVectorSet(0.0f, 1.0f, 1.0f, 1.0f);
	geoGen.createLine(endPointA, endPointB, 79998, colorA, colorB, localSpaceTrans, meshVertices);

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * meshVertices.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vinitData.pSysMem = &meshVertices.Vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshRotTool_zAxisLine_VB));

	meshVertices.Vertices.clear();

	// Angle line.

	localSpaceTrans = XMMatrixIdentity();
	endPointA = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	endPointB = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
	colorA = XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
	colorB = XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
	geoGen.createLine(endPointA, endPointB, 0, colorA, colorB, localSpaceTrans, meshVertices);

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * meshVertices.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vinitData.pSysMem = &meshVertices.Vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshRotTool_angleLine_VB));

	meshVertices.Vertices.clear();

	// XY triangle-list rectangle.

	std::vector<Vertex::PosCol> vertices;
	Vertex::PosCol posCol;
		
	posCol.Col.x = 0.0f; posCol.Col.y = 0.0f; posCol.Col.z = 0.0f; posCol.Col.w = 0.0f; // Transparent.

	// Triangle A...

	posCol.Pos.x = -1.25f; posCol.Pos.y = -1.25f; posCol.Pos.z = 0.0f; //posCol.Pos.x = 0.009f; posCol.Pos.y = 0.009f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);

	posCol.Pos.x = -1.25f; posCol.Pos.y = 1.25f; posCol.Pos.z = 0.0f; // posCol.Pos.x = 0.009f; posCol.Pos.y = 0.99f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);

	posCol.Pos.x = 1.25f; posCol.Pos.y = 1.25f; posCol.Pos.z = 0.0f; // posCol.Pos.x = 0.99f; posCol.Pos.y = 0.99f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);

	// Triangle B...

	posCol.Pos.x = 1.25f; posCol.Pos.y = 1.25f; posCol.Pos.z = 0.0f; // posCol.Pos.x = 0.99f; posCol.Pos.y = 0.99f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);

	posCol.Pos.x = 1.25f; posCol.Pos.y = -1.25f; posCol.Pos.z = 0.0f; // posCol.Pos.x = 0.99f; posCol.Pos.y = 0.009f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);

	posCol.Pos.x = -1.25f; posCol.Pos.y = -1.25f; posCol.Pos.z = 0.0f; // posCol.Pos.x = 0.009f; posCol.Pos.y = 0.009f; posCol.Pos.z = 0.0f;
	vertices.push_back(posCol);

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosCol) * 6;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mMeshRotTool_viewRectangle_VB));
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

	XMMATRIX worldViewProj;

		ID3D11Buffer *buffers[2] = {m_WVPBuffer, m_ColorSchemeIdBuffer};
		md3dImmediateContext->VSSetConstantBuffers(0, 2, buffers);

		// Draw obscuring rectangle.
		XMMATRIX viewControlWorld = XMLoadFloat4x4(&getWorld_viewRectangle_visual());

		float scale = getScale();
		XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);

		worldViewProj = viewControlWorld * camView * camProj;
		worldViewProj = XMMatrixTranspose(worldViewProj);

		md3dImmediateContext->UpdateSubresource(m_WVPBuffer, 0, NULL, &worldViewProj, 0, 0);

		//viewControlWorld = XMLoadFloat4x4(&getWorld_viewRectangle_visual());
		//scaling = XMMatrixScaling(scale, scale, scale);
		//worldViewProj = viewControlWorld * camView * camProj;
		//worldViewProj = XMMatrixTranspose(worldViewProj);
		//md3dImmediateContext->UpdateSubresource(m_WVPBuffer, 0, NULL, &worldViewProj, 0, 0);

		// Billboarded view circle.

		md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		//md3dImmediateContext->OMSetDepthStencilState(RenderStates::AlwaysDSS, 0);
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshRotTool_viewCircle_VB, &stride, &offset);
		md3dImmediateContext->Draw(65, 0);



		md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
					
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
			
		ID3D11BlendState *blendState;
		md3dDevice->CreateBlendState(&blendDesc, &blendState);
			
		float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		UINT sampleMask   = 0xffffffff;
			
		md3dImmediateContext->OMSetBlendState(blendState, NULL, sampleMask);
		md3dImmediateContext->RSSetState(RenderStates::DepthBiasedRS);
			
		ReleaseCOM(blendState);
			
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshRotTool_viewRectangle_VB, &stride, &offset);
		md3dImmediateContext->Draw(6, 0);
			
		md3dImmediateContext->OMSetBlendState(NULL, blendFactor, sampleMask);

		// Angle lines.
		if(xRotationHandle->getIsSelected() || yRotationHandle->getIsSelected() || zRotationHandle->getIsSelected())
		{
			md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshRotTool_angleLine_VB, &stride, &offset);
			md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

			XMVECTOR trans = activeEntity->fetchData<Data::Transform>()->position;
			XMMATRIX translation = XMMatrixTranslationFromVector(trans);
			XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);
			XMVECTOR rotQuat = activeEntity->fetchData<Data::Transform>()->rotation;

			XMVECTOR rotQuatToAnglePointA, rotQuatToAnglePointB;
			omniRotateSphereHandle->getAnglesFromPositiveXUnitAxisToLastAndCurrentlyPickedPoints(rotQuatToAnglePointA, rotQuatToAnglePointB);

			XMMATRIX rotation = XMMatrixRotationQuaternion(XMQuaternionMultiply(rotQuatToAnglePointA, rotQuat));
			XMMATRIX world_angleLine = scaling * rotation * translation * camView * camProj;; //XMLoadFloat4x4(&getWorld_visual());
			world_angleLine = XMMatrixTranspose(world_angleLine);
			md3dImmediateContext->UpdateSubresource(m_WVPBuffer, 0, NULL, &world_angleLine, 0, 0);
			md3dImmediateContext->Draw(2, 0);

			rotation = XMMatrixRotationQuaternion(XMQuaternionMultiply(rotQuatToAnglePointB, rotQuat));
			world_angleLine = scaling * rotation * translation * camView * camProj;; //XMLoadFloat4x4(&getWorld_visual()); //world_angleLine =  translation; XMLoadFloat4x4(&getWorld_visual());
			world_angleLine = XMMatrixTranspose(world_angleLine);
			md3dImmediateContext->UpdateSubresource(m_WVPBuffer, 0, NULL, &world_angleLine, 0, 0);
			md3dImmediateContext->Draw(2, 0);
		}

	// Draw control circles.

	XMFLOAT4X4 toolWorld = getWorld_visual();
	XMMATRIX world = XMLoadFloat4x4(&toolWorld);
	
	worldViewProj = world * camView * camProj;
	worldViewProj = XMMatrixTranspose(worldViewProj);

	md3dImmediateContext->UpdateSubresource(m_WVPBuffer, 0, NULL, &worldViewProj, 0, 0);
	md3dImmediateContext->UpdateSubresource(m_ColorSchemeIdBuffer, 0, NULL, &SETTINGS()->m_ColorScheme_3DManipulatorWidgets, 0, 0);
	md3dImmediateContext->VSSetConstantBuffers(0, 2, buffers);

	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshRotTool_Xcircle_VB, &stride, &offset);
	md3dImmediateContext->Draw(65, 0);

	md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshRotTool_Ycircle_VB, &stride, &offset);
	md3dImmediateContext->Draw(65, 0);

	md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshRotTool_Zcircle_VB, &stride, &offset);
	md3dImmediateContext->Draw(65, 0);


	

	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	toolWorld = getWorld_visual();
	world = XMLoadFloat4x4(&toolWorld);
	worldViewProj = world * camView * camProj;
	worldViewProj = XMMatrixTranspose(worldViewProj);
	md3dImmediateContext->UpdateSubresource(m_WVPBuffer, 0, NULL, &worldViewProj, 0, 0);

	if(yRotationHandle->getIsSelected())
	{
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshRotTool_yAxisLine_VB, &stride, &offset);
		md3dImmediateContext->Draw(80000, 0);
	}
	
	if(xRotationHandle->getIsSelected())
	{
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshRotTool_xAxisLine_VB, &stride, &offset);
		md3dImmediateContext->Draw(80000, 0);
	}
	if(zRotationHandle->getIsSelected())
	{
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshRotTool_zAxisLine_VB, &stride, &offset);
		md3dImmediateContext->Draw(80000, 0);
	}



	
	//md3dImmediateContext->OMSetDepthStencilState(0, 0); // Perhaps unnecessary.
}

