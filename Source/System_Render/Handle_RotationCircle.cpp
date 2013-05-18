#include "stdafx.h"
#include "Handle_RotationCircle.h"

Handle_RotationCircle::Handle_RotationCircle(XMVECTOR &direction, std::vector<XMFLOAT4> boundingLines, char axis)
{
	isSelected = false;

	XMStoreFloat3(&this->direction, direction);
	this->boundingLines = boundingLines;
	
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
	singleAxisRotationPlane = new Handle_RotationPlane(XMLoadFloat3(&dir), 0.0f, boundingRectangle);
}

Handle_RotationCircle::~Handle_RotationCircle()
{
	delete singleAxisRotationPlane;
}

bool static pointVsRectangle(XMVECTOR &point, MyRectangle &rectangle)
{
	bool intersected = false;

	XMVECTOR loadedP1 = XMLoadFloat3(&rectangle.P1); //transformedP1;
	XMVECTOR loadedP2 = XMLoadFloat3(&rectangle.P2); //transformedP2;
	XMVECTOR loadedP3 = XMLoadFloat3(&rectangle.P3); //transformedP3;
	XMVECTOR loadedP4 = XMLoadFloat3(&rectangle.P4); //transformedP4;

	XMVECTOR V1 = loadedP2 - loadedP1;
	XMVECTOR V3 = loadedP4 - loadedP3;
	XMVECTOR V4 = point - loadedP1;
	XMVECTOR V5 = point - loadedP3;

	XMVECTOR V6 = loadedP2 - loadedP3;
	XMVECTOR V7 = loadedP4 - loadedP1;
	XMVECTOR V8 = point - loadedP3;
	XMVECTOR V9 = point - loadedP1;

	V1 = XMVector3Normalize(V1);
	V3 = XMVector3Normalize(V3);
	V4 = XMVector3Normalize(V4);
	V5 = XMVector3Normalize(V5);
	V6 = XMVector3Normalize(V6);
	V7 = XMVector3Normalize(V7);
	V8 = XMVector3Normalize(V8);
	V9 = XMVector3Normalize(V9);

	XMVECTOR V1dotV4 = XMVector3Dot(V1, V4);
	XMVECTOR V3dotV5 = XMVector3Dot(V3, V5);
	XMVECTOR V6dotV8 = XMVector3Dot(V6, V8);
	XMVECTOR V7dotV9 = XMVector3Dot(V7, V9);

	if(		V1dotV4.m128_f32[0] >= 0.0f && V3dotV5.m128_f32[0] >= 0.0f
		&&	V6dotV8.m128_f32[0] >= 0.0f && V7dotV9.m128_f32[0] >= 0.0f	)
	{
		intersected = true;
		//distanceToIntersectionPoint = XMVector3Length(rayOrigin - point).m128_f32[0];
	}

	return intersected;
}

/* Called for initial selection and picking against the axis plane. */
bool Handle_RotationCircle::tryForSelection(MyRectangle &selectionRectangle, XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, XMMATRIX &camProj, float &distanceToIntersectionPoint)
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

	bool theLineIntersectsTheSelectionRectangle = false;
	for(unsigned int i = 0; i < boundingLines.size(); i = i + 2)
	{
		int viewPortHeight = SETTINGS()->windowSize.y;
		int viewPortWidth = SETTINGS()->windowSize.x;

		int DXViewPortTopLeftX = SETTINGS()->DXViewPortTopLeftX;
		int DXViewPortTopLeftY = SETTINGS()->DXViewPortTopLeftY;
		int DXViewPortMinDepth = SETTINGS()->DXViewPortMinDepth;
		int DXViewPortMaxDepth = SETTINGS()->DXViewPortMaxDepth;

		// Project the 3D re-entry point to 2D, so that the screen coordinates can be calculated.
		XMVECTOR lineSegmentPointAScreenPos = XMVector3Project(	XMLoadFloat4(&boundingLines.at(   i   )),
			DXViewPortTopLeftX,	DXViewPortTopLeftY,
			viewPortWidth,		viewPortHeight,
			DXViewPortMinDepth,	DXViewPortMaxDepth,
			camProj, camView, XMLoadFloat4x4(&world)	);

		XMVECTOR lineSegmentPointBScreenPos = XMVector3Project(	XMLoadFloat4(&boundingLines.at( i + 1 )),
			DXViewPortTopLeftX,	DXViewPortTopLeftY,
			viewPortWidth,		viewPortHeight,
			DXViewPortMinDepth,	DXViewPortMaxDepth,
			camProj, camView, XMLoadFloat4x4(&world)	);

		MyRectangle testRect;

		//testRect.P1 = XMFLOAT3(0.0f,	0.0f,	0.0f);
		//testRect.P2 = XMFLOAT3(0.0f,	1.0f,	0.0f);
		//testRect.P3 = XMFLOAT3(1.0f,	1.0f,	0.0f);
		//testRect.P4 = XMFLOAT3(1.0f,	0.0f,	0.0f);

		testRect.P1 = XMFLOAT3(458.0f,	177.0f,	0.0f);
		testRect.P2 = XMFLOAT3(458.0f,	178.0f, 0.0f);
		testRect.P3 = XMFLOAT3(459.0f,	178.0f, 0.0f);
		testRect.P4 = XMFLOAT3(459.0f,	177.0f,	0.0f);

		theLineIntersectsTheSelectionRectangle = pointVsRectangle(lineSegmentPointAScreenPos, selectionRectangle);
		if(theLineIntersectsTheSelectionRectangle)
		{
			break;
		}

		theLineIntersectsTheSelectionRectangle = pointVsRectangle(lineSegmentPointBScreenPos, selectionRectangle);
		if(theLineIntersectsTheSelectionRectangle)
		{
			break;
		}
	}

	// Try for selection against some plane or sphere or whatever, to se the first interaction point.
	
	isSelected = theLineIntersectsTheSelectionRectangle;

	return wasSelected;
}

bool Handle_RotationCircle::pickFirstPointOnAxisPlane(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, float &distanceToPointOfIntersection)
{
	bool pickedAPoint = singleAxisRotationPlane->pickFirstPointOnPlane(rayOrigin, rayDir, camView, distanceToPointOfIntersection);

	return pickedAPoint;
}

		/* The diagram shows an example of the process where: for translations along the single axis Y,
	the X and Z components of the plane’s normal are set to the X and Z values of the initial 
	intersecting (or pick) ray. This has the effect of rotating the plane around the Y axis so
	that the plane’s local XZ components are parallel/co-planar with the local XZ components of
	the viewing plane (in views with orthogonal* projections). The code for this is in the translation 
	controller’s SetPlaneOrientation() function. */
	
/* Called for continued picking against the axis plane, if LMB has yet to be released. */
void Handle_RotationCircle::pickAxisPlane(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView)
{
	if(singleAxisRotationPlane->getIsSelected())
		singleAxisRotationPlane->pickPlane(rayOrigin, rayDir, camView);

	//prevPickedPointOnAxisPlane = nextPickedPointOnAxisPlane;

	//// Calculate where the picking ray that selected the handle intersects with the plane the handle lies in.
	//// ...

	//XMVECTOR loadedDirection = XMLoadFloat3(&direction);
	//XMVECTOR loadedPrevPointOnAxisPlane = XMLoadFloat3(&prevPickedPointOnAxisPlane);
	//XMVECTOR intersectionPoint = loadedPrevPointOnAxisPlane + loadedDirection * 0.1f;

	//XMStoreFloat3(&nextPickedPointOnAxisPlane, intersectionPoint);
}

/* Called when picking against the axis plane should cease, if the LMB has been released. */
void Handle_RotationCircle::unselect()
{
	isSelected = false;
	singleAxisRotationPlane->unselect();
}
	
/* Called to see if this is the currently selected translation axis, if any. */
bool Handle_RotationCircle::getIsSelected()
{
	return isSelected;
}

/* Called to retrieve the last made translation delta. */
XMVECTOR Handle_RotationCircle::getLastTranslationDelta()
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

	if(singleAxisRotationPlane->getIsSelected())
	{
		XMStoreFloat4(&transDelta, singleAxisRotationPlane->getLastTranslationDelta());

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
void Handle_RotationCircle::setWorld(XMMATRIX &world)
{
	XMStoreFloat4x4(&this->world, world);

	singleAxisRotationPlane->setWorld(world);
}

void Handle_RotationCircle::update(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView)
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

