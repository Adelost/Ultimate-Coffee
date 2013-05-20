#include "stdafx.h"
#include "Handle_RotationPlane.h"

Handle_RotationPlane::Handle_RotationPlane(XMVECTOR &direction, std::vector<XMFLOAT4> boundingLines, char axis)
{
	//XMStoreFloat3(&plane.normal, normal);
	//plane.offset = offset;
	//this->boundingRectangle = boundingRectangle;

	sphere.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	sphere.Radius = 1.0f;
}

Handle_RotationPlane::~Handle_RotationPlane()
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

bool Handle_RotationPlane::rayVsRectangle(XMVECTOR &rayOrigin, XMVECTOR &rayDir, MyRectangle &rectangle, float &distanceToIntersectionPoint)
{
	bool rayIntersectedWithRectangle = false;

	// Calculate the definition of the plane that the rectangle is lying in.
	XMVECTOR planeVector = XMPlaneFromPoints(XMLoadFloat3(&rectangle.P1), XMLoadFloat3(&rectangle.P2), XMLoadFloat3(&rectangle.P3));

	MyPlane plane;
	plane.normal.x = planeVector.m128_f32[0];
	plane.normal.y = planeVector.m128_f32[1];
	plane.normal.z = planeVector.m128_f32[2];
	plane.offset = planeVector.m128_f32[3];

	XMVECTOR pointOfIntersection;
	rayVsPlane(rayOrigin, rayDir, plane, pointOfIntersection);

	//XMVECTOR transformedIntersectionPoint = XMVector3Transform(pointOfIntersection, XMLoadFloat4x4(&world));

	XMVECTOR loadedP1 = XMLoadFloat3(&rectangle.P1); 
	XMVECTOR loadedP2 = XMLoadFloat3(&rectangle.P2); 
	XMVECTOR loadedP3 = XMLoadFloat3(&rectangle.P3);
	XMVECTOR loadedP4 = XMLoadFloat3(&rectangle.P4);

	XMVECTOR V1 = loadedP2 - loadedP1;
	XMVECTOR V3 = loadedP4 - loadedP3;
	XMVECTOR V4 = pointOfIntersection - loadedP1;
	XMVECTOR V5 = pointOfIntersection - loadedP3;

	XMVECTOR V6 = loadedP2 - loadedP3;
	XMVECTOR V7 = loadedP4 - loadedP1;
	XMVECTOR V8 = pointOfIntersection - loadedP3;
	XMVECTOR V9 = pointOfIntersection - loadedP1;

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
		rayIntersectedWithRectangle = true;
		distanceToIntersectionPoint = XMVector3Length(rayOrigin - pointOfIntersection).m128_f32[0];
	}

	return rayIntersectedWithRectangle;
}

/* Called for initial selection and picking against the axis plane. */
bool Handle_RotationPlane::tryForSelection(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, float &distanceToIntersectionPoint)
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

	// Calculate if the ray intersects with the plane's rectangle handle.
	isSelected = rayVsRectangle(transRayOrigin, transRayDir, boundingRectangle, distanceToIntersectionPoint);

	if(isSelected)
	{
		// Calculate if and where the ray intersects the translation plane.
		XMVECTOR planeIntersectionPoint;
		bool rayIntersectedWithPlane = rayVsPlane(transRayOrigin, transRayDir, plane, planeIntersectionPoint);
	
		if(rayIntersectedWithPlane)
		{
			//prevPickedPointOnAxisPlane = planeIntersectionPoint;

			XMStoreFloat3(&firstPickedPointOnAxisPlane, planeIntersectionPoint);
			currentlyPickedPointOnAxisPlane = firstPickedPointOnAxisPlane;
		}
	}

	return isSelected;
}

bool Handle_RotationPlane::pickFirstPointOnPlane(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, float &distanceToIntersectionPoint)
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

/* Called for continued picking against the axis plane, if LMB has yet to be released. */
void Handle_RotationPlane::pickPlane(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView)
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
void Handle_RotationPlane::unselect()
{
	isSelected = false;
	firstPickedPointOnAxisPlane.x = 0.0f;
	firstPickedPointOnAxisPlane.y = 0.0f;
	firstPickedPointOnAxisPlane.z = 0.0f;
	currentlyPickedPointOnAxisPlane.x = 0.0f;
	currentlyPickedPointOnAxisPlane.y = 0.0f;
	currentlyPickedPointOnAxisPlane.z = 0.0f;
}
	
/* Called to see if this is the currently selected translation plane, if any. */
bool Handle_RotationPlane::getIsSelected()
{
	return isSelected;
}

/* Called to retrieve the last made translation delta. */
XMVECTOR Handle_RotationPlane::getCurrentRotationQuaternion()
{
	XMVECTOR loadedCurrentlyPickedPointOnAxisPlane = XMLoadFloat3(&currentlyPickedPointOnAxisPlane);
	XMVECTOR loadedFirstPickedPointOnAxisPlane = XMLoadFloat3(&firstPickedPointOnAxisPlane);

	// Calc. the angle between the two picked points.
	XMVECTOR vecFromSphereCenterToFirstPoint = loadedFirstPickedPointOnAxisPlane - XMLoadFloat4(&centerPoint);
	XMVECTOR vecFromSphereCenterToCurrentPoint = loadedCurrentlyPickedPointOnAxisPlane - XMLoadFloat4(&centerPoint);

	XMVECTOR angleVec = XMVector3AngleBetweenVectors(vecFromSphereCenterToFirstPoint, vecFromSphereCenterToCurrentPoint);

	XMVECTOR rotQuat;
	rotQuat = XMQuaternionRotationAxis(XMLoadFloat3(&plane.normal), -angleVec.m128_f32[0]);

	return rotQuat;
}

/* Called for the needed transform of the visual and/or bounding components of the handle. */
void Handle_RotationPlane::setWorld(XMMATRIX &world)
{
	XMStoreFloat4x4(&this->world, world);
}

/* Called to set the plane orientation. */
void Handle_RotationPlane::setPlaneOrientation(XMVECTOR &normal)
{
	XMStoreFloat3(&plane.normal, normal);
}