#include "stdafx.h"
#include "Handle_OmniDimensionalScaling.h"

Handle_OmniDimensionalScaling::Handle_OmniDimensionalScaling(std::vector<XMFLOAT4> boundingTriangles)
{
	isSelected = false;

	this->boundingTriangles = boundingTriangles;

	MyRectangle boundingRectangle; // Dummy rectangle.
	camViewPlane = new Handle_CamViewPlane();

	camViewPlane->setShouldFlipCursorAroundSceneView(true);
}

Handle_OmniDimensionalScaling::~Handle_OmniDimensionalScaling()
{
	delete camViewPlane;
}

/* Called for initial selection and picking against the axis plane. */
bool Handle_OmniDimensionalScaling::tryForSelection(MyRectangle &selectionRectangle, XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, float &distanceToIntersectionPoint, POINT &mouseCursorPoint)
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
			XMVECTOR normalizedRayDir = XMVector3Normalize(rayDir);

			camViewPlane->setFirstPointOnViewPlane(mouseCursorPoint);

			distanceToIntersectionPoint = dist;
			break;
		}
	}

	isSelected = wasSelected;

	return wasSelected;
}

//bool Handle_OmniDimensionalScaling::pickFirstPointOnViewPlane(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, float &distanceToPointOfIntersection)
//{
//	bool pickedAPoint = singleAxisScalingPlane->pickFirstPointOnPlane(rayOrigin, rayDir, camView, distanceToPointOfIntersection);
//
//	return pickedAPoint;
//}

		/* The diagram shows an example of the process where: for translations along the single axis Y,
	the X and Z components of the plane’s normal are set to the X and Z values of the initial 
	intersecting (or pick) ray. This has the effect of rotating the plane around the Y axis so
	that the plane’s local XZ components are parallel/co-planar with the local XZ components of
	the viewing plane (in views with orthogonal* projections). The code for this is in the translation 
	controller’s SetPlaneOrientation() function. */
	
/* Called for continued picking against the axis plane, if LMB has yet to be released. */
//void Handle_OmniDimensionalScaling::pickAxisPlane(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, XMMATRIX &camProj)
//{
//	D3D11_VIEWPORT dummy;
//	if(singleAxisScalingPlane->getIsSelected())
//		singleAxisScalingPlane->pickPlane(rayOrigin, rayDir, camView, camProj, dummy);
//
//	//prevPickedPointOnAxisPlane = nextPickedPointOnAxisPlane;
//
//	//// Calculate where the picking ray that selected the handle intersects with the plane the handle lies in.
//	//// ...
//
//	//XMVECTOR loadedDirection = XMLoadFloat3(&direction);
//	//XMVECTOR loadedPrevPointOnAxisPlane = XMLoadFloat3(&prevPickedPointOnAxisPlane);
//	//XMVECTOR intersectionPoint = loadedPrevPointOnAxisPlane + loadedDirection * 0.1f;
//
//	//XMStoreFloat3(&nextPickedPointOnAxisPlane, intersectionPoint);
//}

/* Called when picking against the axis plane should cease, if the LMB has been released. */
void Handle_OmniDimensionalScaling::unselect()
{
	isSelected = false;
	camViewPlane->unselect();
}
	
/* Called to see if this is the currently selected translation axis, if any. */
bool Handle_OmniDimensionalScaling::getIsSelected()
{
	return isSelected;
}

void Handle_OmniDimensionalScaling::calcLastScalingDelta()
{
	//camViewPlane->calcLastScalingDelta();
}

POINT Handle_OmniDimensionalScaling::getTotalScalingDelta()
{
	POINT totalScalingXYDeltas;
	totalScalingXYDeltas.x = 0;
	totalScalingXYDeltas.y = 0;

	if(camViewPlane->getIsSelected())
	{
		totalScalingXYDeltas = camViewPlane->getTotalMouseCursorXYDeltas();
	}

	return totalScalingXYDeltas;
}

/* Called to retrieve the last made translation delta. */
//POINT Handle_OmniDimensionalScaling::getLastTranslationDelta()
//{
	////XMVECTOR loadedNextPickedPointOnAxisPlane; XMLoadFloat3(&nextPickedPointOnAxisPlane);
	////XMVECTOR loadedPrevPickedPointOnAxisPlane; XMLoadFloat3(&prevPickedPointOnAxisPlane);
	////XMVECTOR diffVector = loadedNextPickedPointOnAxisPlane - loadedPrevPickedPointOnAxisPlane;
	////
	////XMVECTOR loadedDirection = XMLoadFloat3(&direction);
	////XMFLOAT3 tempVector(	loadedDirection.m128_f32[0] * diffVector.m128_f32[0],
	////						loadedDirection.m128_f32[1] * diffVector.m128_f32[1],
	////						loadedDirection.m128_f32[2] * diffVector.m128_f32[2]	);
	////
	////return XMLoadFloat3(&tempVector);

	//XMFLOAT4 transDelta;
	//XMVECTOR oneAxisOnlyTransDelta = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	//if(singleAxisScalingPlane->getIsSelected())
	//{
	//	XMStoreFloat4(&transDelta, singleAxisScalingPlane->getTotalScalingDelta());

	//	if(direction.x == 1.0f)
	//	{
	//		transDelta.y = 0.0f; transDelta.z = 0.0f;
	//	}
	//	else if(direction.y == 1.0f)
	//	{
	//		transDelta.x = 0.0f; transDelta.z = 0.0f;
	//	}
	//	else if(direction.z == 1.0f)
	//	{
	//		transDelta.x = 0.0f; transDelta.y = 0.0f;
	//	}
	//	
	//	oneAxisOnlyTransDelta = XMLoadFloat4(&transDelta);
	//}

	//return oneAxisOnlyTransDelta;
//}

/* Called for the needed transform of the visual and/or bounding components of the handle. */
void Handle_OmniDimensionalScaling::setWorld(XMMATRIX &world)
{
	XMStoreFloat4x4(&this->world, world);
}

void Handle_OmniDimensionalScaling::update(POINT &mouseCursorPoint)
{
	camViewPlane->update(mouseCursorPoint);

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