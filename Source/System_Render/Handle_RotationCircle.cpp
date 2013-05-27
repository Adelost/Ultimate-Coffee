#include "stdafx.h"
#include "Handle_RotationCircle.h"

Handle_RotationCircle::Handle_RotationCircle(XMVECTOR &direction, std::vector<XMFLOAT4> boundingLines, char axis)
{
	isSelected = false;

	sphere.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	sphere.Radius = 1.0f;

	XMStoreFloat3(&this->direction, direction);
	this->boundingLines = boundingLines;
	
	XMFLOAT3 dir;

	switch (axis)
	{
		case 'x': 
			XMStoreFloat3(&plane.normal, XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f));
			break;
		case 'y':
			XMStoreFloat3(&plane.normal, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
			break;
		case 'z':
			XMStoreFloat3(&plane.normal, XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
			break;
		default:
			throw "Error in Handle_TranslationAxis constructor: Axis not specified correctly.";
	}
}

Handle_RotationCircle::~Handle_RotationCircle()
{
}

static bool rayVsPlane(XMVECTOR &rayOrigin, XMVECTOR &rayDir, MyPlane &plane, XMVECTOR &pointOfIntersection)
{
	bool rayIntersectedWithPlane = false;

	// Make the ray direction unit length for the intersection tests.
	rayDir = XMVector3Normalize(rayDir);

	XMVECTOR loadedPlaneNormal = XMLoadFloat3(&plane.normal);
	float t = -(XMVector3Dot(rayOrigin, loadedPlaneNormal).m128_f32[0]
			  + plane.offset) / XMVector3Dot(rayDir, loadedPlaneNormal).m128_f32[0];

	rayIntersectedWithPlane = true; // TEST: We always intersect.
	pointOfIntersection = rayOrigin + t * rayDir;

	return rayIntersectedWithPlane;
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

bool static lineVsLine(XMVECTOR &lineA_pointP, XMVECTOR &lineA_pointR, XMVECTOR &lineB_pointQ, XMVECTOR &lineB_pointS)
{
	bool intersection = false;

	XMVECTOR test;

	float t = (XMVector2Cross((lineB_pointQ - lineA_pointP), lineB_pointS - lineB_pointQ) / XMVector2Cross(lineA_pointR - lineA_pointP, lineB_pointS - lineB_pointQ)).m128_f32[0];
	
	float u = (XMVector2Cross((lineB_pointQ - lineA_pointP), lineA_pointR - lineA_pointP) / XMVector2Cross(lineA_pointR - lineA_pointP, lineB_pointS - lineB_pointQ)).m128_f32[0];

	if(		t >= 0.0f && t <= 1.0f
		&&	u >= 0.0f && u <= 1.0f	)
	{
		intersection = true;
	}

	return intersection;
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

	int viewPortHeight = SETTINGS()->windowSize.y;
	int viewPortWidth = SETTINGS()->windowSize.x;

	int DXViewPortTopLeftX = SETTINGS()->DXViewPortTopLeftX;
	int DXViewPortTopLeftY = SETTINGS()->DXViewPortTopLeftY;
	int DXViewPortMinDepth = SETTINGS()->DXViewPortMinDepth;
	int DXViewPortMaxDepth = SETTINGS()->DXViewPortMaxDepth;

	for(unsigned int i = 0; i < boundingLines.size(); i = i + 2)
	{
		XMVECTOR linePointA = XMLoadFloat4(&boundingLines.at(   i   ));
		XMVECTOR linePointB = XMLoadFloat4(&boundingLines.at( i + 1 ));
		XMVECTOR avgLinePointPos = (linePointA + linePointB) / 2.0f;

		avgLinePointPos = XMVector3Transform(linePointA, XMLoadFloat4x4(&world));

//* first normalize your plane normal (like show above), put it into N
//* compute D, using a arbitrary point P, that lies on the plane: D = - (Nx*Px + Ny*Py + Nz*Pz); Don't forget the inversion !
//* Now you have the normalized plane equation factors: (N,D)

		// Check if the "average line point" is currently behind the selection blocking plane.
		float D = -(selectionBlockingPlaneNormal.x * world._41 + selectionBlockingPlaneNormal.y * world._42 + selectionBlockingPlaneNormal.z * world._43);
		float res =	  selectionBlockingPlaneNormal.x * avgLinePointPos.m128_f32[0]
					+ selectionBlockingPlaneNormal.y * avgLinePointPos.m128_f32[1]
					+ selectionBlockingPlaneNormal.z * avgLinePointPos.m128_f32[2] + D; //0.0f; //0.0f;

					float test = XMVector3Dot(avgLinePointPos, XMLoadFloat3(&selectionBlockingPlaneNormal)).m128_f32[0];

		if(res >= 0.0f)
		{
			// Project the 3D re-entry point to 2D, so that the screen coordinates can be calculated.
			XMVECTOR lineSegmentPointAScreenPos = XMVector3Project(linePointA,
				DXViewPortTopLeftX,	DXViewPortTopLeftY,
				viewPortWidth,		viewPortHeight,
				DXViewPortMinDepth,	DXViewPortMaxDepth,
				camProj, camView, XMLoadFloat4x4(&world)	);

			XMVECTOR lineSegmentPointBScreenPos = XMVector3Project(linePointB,
				DXViewPortTopLeftX,	DXViewPortTopLeftY,
				viewPortWidth,		viewPortHeight,
				DXViewPortMinDepth,	DXViewPortMaxDepth,
				camProj, camView, XMLoadFloat4x4(&world)	);

			MyRectangle testRect;

			//testRect.P1 = XMFLOAT3(458.0f,	177.0f,	0.0f);
			//testRect.P2 = XMFLOAT3(458.0f,	178.0f, 0.0f);
			//testRect.P3 = XMFLOAT3(459.0f,	178.0f, 0.0f);
			//testRect.P4 = XMFLOAT3(459.0f,	177.0f,	0.0f);

			theLineIntersectsTheSelectionRectangle = lineVsLine(lineSegmentPointAScreenPos, lineSegmentPointBScreenPos, XMLoadFloat3(&selectionRectangle.P1), XMLoadFloat3(&selectionRectangle.P2)); //theLineIntersectsTheSelectionRectangle = pointVsRectangle(lineSegmentPointAScreenPos, selectionRectangle);
			if(theLineIntersectsTheSelectionRectangle)
			{
				break;
			}

			theLineIntersectsTheSelectionRectangle = lineVsLine(lineSegmentPointAScreenPos, lineSegmentPointBScreenPos, XMLoadFloat3(&selectionRectangle.P2), XMLoadFloat3(&selectionRectangle.P3)); //pointVsRectangle(lineSegmentPointBScreenPos, selectionRectangle);
			if(theLineIntersectsTheSelectionRectangle)
			{
				break;
			}

			theLineIntersectsTheSelectionRectangle = lineVsLine(lineSegmentPointAScreenPos, lineSegmentPointBScreenPos, XMLoadFloat3(&selectionRectangle.P3), XMLoadFloat3(&selectionRectangle.P4)); //pointVsRectangle(lineSegmentPointBScreenPos, selectionRectangle);
			if(theLineIntersectsTheSelectionRectangle)
			{
				break;
			}

			theLineIntersectsTheSelectionRectangle = lineVsLine(lineSegmentPointAScreenPos, lineSegmentPointBScreenPos, XMLoadFloat3(&selectionRectangle.P4), XMLoadFloat3(&selectionRectangle.P1)); //pointVsRectangle(lineSegmentPointBScreenPos, selectionRectangle);
			if(theLineIntersectsTheSelectionRectangle)
			{
				break;
			}
		}
	}

	// Try for selection against the rotation plane, to pick the first point on the plane.
	float dist;
	pickFirstPointOnAxisPlane(transRayOrigin, transRayDir, camView, dist);

	isSelected = theLineIntersectsTheSelectionRectangle;

	return isSelected;
}

bool Handle_RotationCircle::pickFirstPointOnAxisPlane(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, float &distanceToPointOfIntersection)
{
	isSelected = false;

	// Tranform ray to local space.
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(camView), camView);

	XMMATRIX W = XMLoadFloat4x4(&world);
	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

	XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

	XMVECTOR transRayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
	XMVECTOR transRayDir = XMVector3TransformNormal(rayDir, toLocal);

	// Make the ray direction unit length for the intersection tests.
	transRayDir = XMVector3Normalize(transRayDir);
	
	// Calculate if and where the ray intersects the translation plane.
	XMVECTOR planeIntersectionPoint;
	bool rayIntersectedWithPlane = rayVsPlane(transRayOrigin, transRayDir, plane, planeIntersectionPoint);
	
	if(rayIntersectedWithPlane)
	{
		//prevPickedPointOnAxisPlane = planeIntersectionPoint;

		XMStoreFloat3(&firstPickedPointOnAxisPlane, planeIntersectionPoint);
		currentlyPickedPointOnAxisPlane = firstPickedPointOnAxisPlane;

		isSelected = rayIntersectedWithPlane;
	}

	return isSelected;
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
	// Tranform ray to local space.
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(camView), camView);

	XMMATRIX W = XMLoadFloat4x4(&world);
	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

	XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

	XMVECTOR transRayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
	XMVECTOR transRayDir = XMVector3TransformNormal(rayDir, toLocal);

	// Make the ray direction unit length for the intersection tests.
	transRayDir = XMVector3Normalize(transRayDir);
	
	XMVECTOR planeIntersectionPoint;
	bool rayIntersectedWithPlane = rayVsPlane(transRayOrigin, transRayDir, plane, planeIntersectionPoint);

	XMStoreFloat3(&currentlyPickedPointOnAxisPlane, planeIntersectionPoint);
}

/* Called when picking against the axis plane should cease, if the LMB has been released. */
void Handle_RotationCircle::unselect()
{
	isSelected = false;
}
	
/* Called to see if this is the currently selected translation axis, if any. */
bool Handle_RotationCircle::getIsSelected()
{
	return isSelected;
}

/* Called to retrieve the last made translation delta. */
XMVECTOR Handle_RotationCircle::getCurrentRotationQuaternion()
{
	XMVECTOR loadedCurrentlyPickedPointOnAxisPlane = XMLoadFloat3(&currentlyPickedPointOnAxisPlane);
	XMVECTOR loadedFirstPickedPointOnAxisPlane = XMLoadFloat3(&firstPickedPointOnAxisPlane);

	// Calc. the angle between the two picked points.
	XMVECTOR vecFromSphereCenterToFirstPoint = loadedFirstPickedPointOnAxisPlane - XMLoadFloat3(&sphere.Center);
	XMVECTOR vecFromSphereCenterToCurrentPoint = loadedCurrentlyPickedPointOnAxisPlane - XMLoadFloat3(&sphere.Center);

	XMVECTOR angleVec = XMVector3AngleBetweenVectors(vecFromSphereCenterToFirstPoint, vecFromSphereCenterToCurrentPoint);

	XMVECTOR rotQuat;
	rotQuat = XMQuaternionRotationAxis(XMLoadFloat3(&plane.normal), -angleVec.m128_f32[0]);

	return rotQuat;
}

/* Called for the needed transform of the visual and/or bounding components of the handle. */
void Handle_RotationCircle::setWorld(XMMATRIX &world)
{
	XMStoreFloat4x4(&this->world, world);
	
	XMVECTOR transformedNormal = XMVector3Transform(XMLoadFloat3(&plane.normal), world);

	XMStoreFloat3(&plane.normal, transformedNormal);
}

void Handle_RotationCircle::setPlaneOrientation(XMVECTOR &normal)
{
	XMStoreFloat3(&plane.normal, normal);
}

void Handle_RotationCircle::update(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView)
{
}

void Handle_RotationCircle::setSelectionBlockingPlaneNormal(XMVECTOR &normal)
{
	//XMVECTOR transformedNormal = XMVector3Transform(normal, XMLoadFloat4x4(world));
	//XMStoreFloat3(&selectionBlockingPlaneNormal, transformedNormal);
	XMStoreFloat3(&selectionBlockingPlaneNormal, normal);
}