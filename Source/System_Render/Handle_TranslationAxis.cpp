#include "stdafx.h"
#include "Handle_TranslationAxis.h"

Handle_TranslationAxis::Handle_TranslationAxis(XMVECTOR &direction, std::vector<XMFLOAT4> boundingTriangles, char axis)
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
			throw "Error in Handle_TranslationAxis constructor: Axis not specified correctly.";
	}

	MyRectangle boundingRectangle; // Dummy rectangle.
	singleAxisTranslationPlane = new Handle_TranslationPlane(XMLoadFloat3(&dir), 0.0f, boundingRectangle);
}

Handle_TranslationAxis::~Handle_TranslationAxis()
{
	delete singleAxisTranslationPlane;
}

/* Called for initial selection and picking against the axis plane. */
bool Handle_TranslationAxis::tryForSelection(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, float &distanceToIntersectionPoint)
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

			XMFLOAT3 reorientedNormal;

			if(direction.x == 1.0f)
				reorientedNormal = XMFLOAT3(0.0f, normalizedRayDir.m128_f32[1], normalizedRayDir.m128_f32[2]);
			else if(direction.y == 1.0f)
				reorientedNormal = XMFLOAT3(normalizedRayDir.m128_f32[0], 0.0f, normalizedRayDir.m128_f32[2]);
			else if(direction.z == 1.0f)
				reorientedNormal = XMFLOAT3(normalizedRayDir.m128_f32[0], normalizedRayDir.m128_f32[1], 0.0f);

			singleAxisTranslationPlane->setPlaneOrientation(XMLoadFloat3(&reorientedNormal));

			pickFirstPointOnAxisPlane(rayOrigin, rayDir, camView, distanceToIntersectionPoint);
			distanceToIntersectionPoint = dist;
			break;
		}
	}

	isSelected = wasSelected;

	return wasSelected;
}

bool Handle_TranslationAxis::pickFirstPointOnAxisPlane(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, float &distanceToPointOfIntersection)
{
	bool pickedAPoint = singleAxisTranslationPlane->pickFirstPointOnPlane(rayOrigin, rayDir, camView, distanceToPointOfIntersection);

	return pickedAPoint;
}

		/* The diagram shows an example of the process where: for translations along the single axis Y,
	the X and Z components of the plane’s normal are set to the X and Z values of the initial 
	intersecting (or pick) ray. This has the effect of rotating the plane around the Y axis so
	that the plane’s local XZ components are parallel/co-planar with the local XZ components of
	the viewing plane (in views with orthogonal* projections). The code for this is in the translation 
	controller’s SetPlaneOrientation() function. */
	
/* Called for continued picking against the axis plane, if LMB has yet to be released. */
void Handle_TranslationAxis::pickAxisPlane(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView)
{
	if(singleAxisTranslationPlane->getIsSelected())
		singleAxisTranslationPlane->pickPlane(rayOrigin, rayDir, camView);

	//prevPickedPointOnAxisPlane = nextPickedPointOnAxisPlane;

	//// Calculate where the picking ray that selected the handle intersects with the plane the handle lies in.
	//// ...

	//XMVECTOR loadedDirection = XMLoadFloat3(&direction);
	//XMVECTOR loadedPrevPointOnAxisPlane = XMLoadFloat3(&prevPickedPointOnAxisPlane);
	//XMVECTOR intersectionPoint = loadedPrevPointOnAxisPlane + loadedDirection * 0.1f;

	//XMStoreFloat3(&nextPickedPointOnAxisPlane, intersectionPoint);
}

/* Called when picking against the axis plane should cease, if the LMB has been released. */
void Handle_TranslationAxis::unselect()
{
	isSelected = false;
	singleAxisTranslationPlane->unselect();
}
	
/* Called to see if this is the currently selected translation axis, if any. */
bool Handle_TranslationAxis::getIsSelected()
{
	return isSelected;
}

/* Called to retrieve the last made translation delta. */
XMVECTOR Handle_TranslationAxis::getLastTranslationDelta()
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

	if(singleAxisTranslationPlane->getIsSelected())
	{
		XMStoreFloat4(&transDelta, singleAxisTranslationPlane->getLastTranslationDelta());

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
void Handle_TranslationAxis::setWorld(XMMATRIX &world)
{
	XMStoreFloat4x4(&this->world, world);

	singleAxisTranslationPlane->setWorld(world);
}

void Handle_TranslationAxis::update(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView)
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

