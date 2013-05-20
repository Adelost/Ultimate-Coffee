#include "stdafx.h"
#include "Handle_ScalingAxis.h"

Handle_ScalingAxis::Handle_ScalingAxis(XMVECTOR &direction, std::vector<XMFLOAT4> boundingTriangles, char axis)
{
	isSelected = false;

	XMStoreFloat3(&this->direction, direction);
	this->boundingTriangles = boundingTriangles;
	
	XMFLOAT3 dir;

	switch (axis)
	{
		case 'x': 
			dir = XMFLOAT3(1.0f, 0.0f, 0.0f);
			break;
		case 'y':
			dir = XMFLOAT3(0.0f, 1.0f, 0.0f);
			break;
		case 'z':
			dir = XMFLOAT3(0.0f, 0.0f, 1.0f);
			break;
		default:
			throw "Error in Handle_ScalingAxis constructor: Axis not specified correctly.";
	}

	DimensionTuple dummyTuple = XY;
	MyRectangle boundingRectangle; // Dummy rectangle.
	singleAxisScalingPlane = new Handle_ScalingPlane(XMLoadFloat3(&dir), 0.0f, boundingRectangle, dummyTuple);
	singleAxisScalingPlane->setShouldFlipMouseCursor(false);
}

Handle_ScalingAxis::~Handle_ScalingAxis()
{
	delete singleAxisScalingPlane;
}

/* Called for initial selection and picking against the axis plane. */
bool Handle_ScalingAxis::tryForSelection(MyRectangle &selectionRectangle, XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, float &distanceToIntersectionPoint)
{
	bool wasSelected = false;

	// Tranform ray to local space.
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(camView), camView);

	XMMATRIX W = XMLoadFloat4x4(&world);
	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

	XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

	XMVECTOR transRayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
	XMVECTOR transRayDir = XMVector3TransformNormal(rayDir, toLocal);

	// Make the ray direction unit length for the intersection tests.
	transRayDir = XMVector3Normalize(transRayDir);

	float dist;
	for(unsigned int i = 0; i < boundingTriangles.size(); i = i + 3)
	{
		wasSelected = XNA::IntersectRayTriangle(transRayOrigin, transRayDir, XMLoadFloat4(&boundingTriangles.at(i)), XMLoadFloat4(&boundingTriangles.at(i + 1)), XMLoadFloat4(&boundingTriangles.at(i + 2)), &dist);

		if(wasSelected)
		{
			XMVECTOR normalizedRayDir = XMVector3Normalize(transRayDir);

			XMFLOAT3 reorientedNormal;

			if(direction.x == 1.0f)
				reorientedNormal = XMFLOAT3(0.0f, normalizedRayDir.m128_f32[1], normalizedRayDir.m128_f32[2]);
			else if(direction.y == 1.0f)
				reorientedNormal = XMFLOAT3(normalizedRayDir.m128_f32[0], 0.0f, normalizedRayDir.m128_f32[2]);
			else if(direction.z == 1.0f)
				reorientedNormal = XMFLOAT3(normalizedRayDir.m128_f32[0], normalizedRayDir.m128_f32[1], 0.0f);

			singleAxisScalingPlane->setPlaneOrientation(XMLoadFloat3(&reorientedNormal));

			pickFirstPointOnAxisPlane(rayOrigin, rayDir, camView, distanceToIntersectionPoint);
			distanceToIntersectionPoint = dist;
			break;
		}
	}

	isSelected = wasSelected;

	return wasSelected;
}

bool Handle_ScalingAxis::pickFirstPointOnAxisPlane(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, float &distanceToPointOfIntersection)
{
	bool pickedAPoint = singleAxisScalingPlane->pickFirstPointOnPlane(rayOrigin, rayDir, camView, distanceToPointOfIntersection);

	return pickedAPoint;
}

		/* The diagram shows an example of the process where: for translations along the single axis Y,
	the X and Z components of the plane’s normal are set to the X and Z values of the initial 
	intersecting (or pick) ray. This has the effect of rotating the plane around the Y axis so
	that the plane’s local XZ components are parallel/co-planar with the local XZ components of
	the viewing plane (in views with orthogonal* projections). The code for this is in the translation 
	controller’s SetPlaneOrientation() function. */
	
/* Called for continued picking against the axis plane, if LMB has yet to be released. */
void Handle_ScalingAxis::pickAxisPlane(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, XMMATRIX &camProj)
{
	D3D11_VIEWPORT dummy;
	if(singleAxisScalingPlane->getIsSelected())
		singleAxisScalingPlane->pickPlane(rayOrigin, rayDir, camView, camProj, dummy);

	//prevPickedPointOnAxisPlane = nextPickedPointOnAxisPlane;

	//// Calculate where the picking ray that selected the handle intersects with the plane the handle lies in.
	//// ...

	//XMVECTOR loadedDirection = XMLoadFloat3(&direction);
	//XMVECTOR loadedPrevPointOnAxisPlane = XMLoadFloat3(&prevPickedPointOnAxisPlane);
	//XMVECTOR intersectionPoint = loadedPrevPointOnAxisPlane + loadedDirection * 0.1f;

	//XMStoreFloat3(&nextPickedPointOnAxisPlane, intersectionPoint);
}

/* Called when picking against the axis plane should cease, if the LMB has been released. */
void Handle_ScalingAxis::unselect()
{
	isSelected = false;
	singleAxisScalingPlane->unselect();
}
	
/* Called to see if this is the currently selected translation axis, if any. */
bool Handle_ScalingAxis::getIsSelected()
{
	return isSelected;
}

void Handle_ScalingAxis::calcLastScalingDelta()
{
	singleAxisScalingPlane->calcLastScalingDelta();
}

XMVECTOR Handle_ScalingAxis::getTotalScalingDelta()
{
	XMFLOAT4 transDelta;
	XMVECTOR oneAxisOnlyTransDelta = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	if(singleAxisScalingPlane->getIsSelected())
	{
		XMStoreFloat4(&transDelta, singleAxisScalingPlane->getTotalScalingDelta());

		if(direction.x == 1.0f)
		{
			transDelta.y = 0.0f; transDelta.z = 0.0f;
		}
		else if(direction.y == 1.0f)
		{
			transDelta.x = 0.0f; transDelta.z = 0.0f;
		}
		else if(direction.z == 1.0f)
		{
			transDelta.x = 0.0f; transDelta.y = 0.0f;
		}
		
		oneAxisOnlyTransDelta = XMLoadFloat4(&transDelta);
	}

	return oneAxisOnlyTransDelta;
}

/* Called to retrieve the last made translation delta. */
XMVECTOR Handle_ScalingAxis::getLastTranslationDelta()
{
	//XMVECTOR loadedNextPickedPointOnAxisPlane; XMLoadFloat3(&nextPickedPointOnAxisPlane);
	//XMVECTOR loadedPrevPickedPointOnAxisPlane; XMLoadFloat3(&prevPickedPointOnAxisPlane);
	//XMVECTOR diffVector = loadedNextPickedPointOnAxisPlane - loadedPrevPickedPointOnAxisPlane;
	//
	//XMVECTOR loadedDirection = XMLoadFloat3(&direction);
	//XMFLOAT3 tempVector(	loadedDirection.m128_f32[0] * diffVector.m128_f32[0],
	//						loadedDirection.m128_f32[1] * diffVector.m128_f32[1],
	//						loadedDirection.m128_f32[2] * diffVector.m128_f32[2]	);
	//
	//return XMLoadFloat3(&tempVector);

	XMFLOAT4 transDelta;
	XMVECTOR oneAxisOnlyTransDelta = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	if(singleAxisScalingPlane->getIsSelected())
	{
		XMStoreFloat4(&transDelta, singleAxisScalingPlane->getTotalScalingDelta());

		if(direction.x == 1.0f)
		{
			transDelta.y = 0.0f; transDelta.z = 0.0f;
		}
		else if(direction.y == 1.0f)
		{
			transDelta.x = 0.0f; transDelta.z = 0.0f;
		}
		else if(direction.z == 1.0f)
		{
			transDelta.x = 0.0f; transDelta.y = 0.0f;
		}
		
		oneAxisOnlyTransDelta = XMLoadFloat4(&transDelta);
	}

	return oneAxisOnlyTransDelta;
}

/* Called for the needed transform of the visual and/or bounding components of the handle. */
void Handle_ScalingAxis::setWorld(XMMATRIX &world)
{
	XMStoreFloat4x4(&this->world, world);

	singleAxisScalingPlane->setWorld(world);
}

void Handle_ScalingAxis::update(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView)
{
	//// Tranform ray to local space.
	//XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(camView), camView);

	//XMMATRIX W = XMLoadFloat4x4(&world);
	//XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

	//XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

	//XMVECTOR transRayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
	//XMVECTOR transRayDir = XMVector3TransformNormal(rayDir, toLocal);

	//// Make the ray direction unit length for the intersection tests.
	//transRayDir = XMVector3Normalize(transRayDir);
}

