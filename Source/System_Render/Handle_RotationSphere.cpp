#include "Handle_RotationSphere.h"

Handle_RotationSphere::Handle_RotationSphere(XMVECTOR center, float radius, HWND windowHandle)
{
	XMStoreFloat3(&sphere.Center, center);
	sphere.Radius = radius;

	this->windowHandle = windowHandle;
}

Handle_RotationSphere::~Handle_RotationSphere()
{
}

/* Called for initial selection and picking against the sphere. */
bool Handle_RotationSphere::tryForSelection(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, float &distanceToIntersectionPoint)
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
	isSelected = XNA::IntersectRaySphere(transRayOrigin, transRayDir, &sphere, &distanceToIntersectionPoint);

	if(isSelected)
	{
		// Calculate the 3D coordinate at which the ray intersected the sphere.
		XMVECTOR sphereIntersectionPoint;
		sphereIntersectionPoint = transRayOrigin + transRayDir * distanceToIntersectionPoint;

		XMStoreFloat3(&firstPickedPointOnSphere, sphereIntersectionPoint);
		currentlyPickedPointOnSphere = firstPickedPointOnSphere;
	}

	return isSelected;
}

XMVECTOR Handle_RotationSphere::calcRotationAxisFromTwoPointsOnSphere(Sphere &sphere, XMVECTOR &pointA, XMVECTOR &pointB)
{
	XMVECTOR vecFromSphereCenterToA = pointB - XMLoadFloat3(&sphere.Center);
	XMVECTOR vecFromSphereCenterToB = pointA - XMLoadFloat3(&sphere.Center);

	XMVECTOR rotationAxis = XMVector3Cross(vecFromSphereCenterToA, vecFromSphereCenterToB);

	return XMVector3Normalize(rotationAxis);
}

float Handle_RotationSphere::calcAngleBetweenTwoPointsOnSphere(Sphere &sphere, XMVECTOR &pointA, XMVECTOR &pointB)
{
	XMVECTOR vecFromSphereCenterToA = pointA - XMLoadFloat3(&sphere.Center);
	XMVECTOR vecFromSphereCenterToB = pointB - XMLoadFloat3(&sphere.Center);

	XMVECTOR angleVec = XMVector3AngleBetweenVectors(vecFromSphereCenterToA, vecFromSphereCenterToB);

	return angleVec.m128_f32[0];
}

/* Called for continued picking against the axis plane, if LMB has yet to be released. */
void Handle_RotationSphere::pickSphere(XMVECTOR &rayOrigin, XMVECTOR &rayDir, Camera &theCamera, D3D11_VIEWPORT &theViewport, POINT &mouseCursorPoint)
{
	// Tranform ray to local space.
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(theCamera.View()), theCamera.View());

	XMMATRIX W = XMLoadFloat4x4(&world);
	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

	XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

	XMVECTOR transRayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
	XMVECTOR transRayDir = XMVector3TransformNormal(rayDir, toLocal);

	// Make the ray direction unit length for the intersection tests.
	transRayDir = XMVector3Normalize(transRayDir);

	float distanceToIntersectionPoint;

	bool rayIntersectedWithPlane = XNA::IntersectRaySphere(transRayOrigin, transRayDir, &sphere, &distanceToIntersectionPoint);

	if(rayIntersectedWithPlane)
	{
		// Calculate the 3D coordinate at which the ray intersected the sphere.
		XMVECTOR sphereIntersectionPoint;
		sphereIntersectionPoint = transRayOrigin + transRayDir * distanceToIntersectionPoint;

		XMStoreFloat3(&currentlyPickedPointOnSphere, sphereIntersectionPoint);

		calcLastRotationQuaternion();
	}
	else
	{
		/*	Since the sphere has been selected and the mouse pointer has since been moved,
			missing the sphere now would mean that the cursor has moved past its boundary,
			and, so, the mouse pointer is to be "looped around" to the opposite boundary point.	*/

		// Take the quaternion rotation made before the coming "spin-around" into account.
		XMStoreFloat4(&quaternionRotationMadeSoFar, XMQuaternionMultiply(XMLoadFloat4(&quaternionRotationMadeSoFar), XMLoadFloat4(&lastQuaternionRotationMade))); //getLastRotationQuaternion()));

		XMStoreFloat4(&lastQuaternionRotationMade, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

		XMVECTOR exitPointVector = XMLoadFloat3(&currentlyPickedPointOnSphere); // - XMLoadFloat3(&sphere.Center);
		XMVECTOR reEntryPointVector = -exitPointVector;
		
		// Project the 3D re-entry point point to 2D, so that the screen coordinates can be calculated.
		XMVECTOR newCursorScreenPos_clientSpace = XMVector3Project(	reEntryPointVector,
																	theViewport.TopLeftX,	theViewport.TopLeftY,
																	theViewport.Width,		theViewport.Height,
																	theViewport.MinDepth,	theViewport.MaxDepth,
																	theCamera.Proj(), theCamera.View(), XMLoadFloat4x4(&world)	);

		// Put the client-space coordinates in a POINT struct and convert it to screen-space coordinates...
		POINT newMouseCursorPoint;
		newMouseCursorPoint.x = newCursorScreenPos_clientSpace.m128_f32[0];
		newMouseCursorPoint.y = newCursorScreenPos_clientSpace.m128_f32[1];
		ClientToScreen(windowHandle, &newMouseCursorPoint);

		// ... then set the mouse cursor's position to the result (x, y) coordinates.
		SetCursorPos(newMouseCursorPoint.x, newMouseCursorPoint.y);

		// Since the cursor position has changed, the picking ray/origin, is updated below...

		XMMATRIX P = theCamera.Proj();

		// Compute picking ray in view space, using the client-space coordinates.
		float vx = (+2.0f * newCursorScreenPos_clientSpace.m128_f32[0] / theViewport.Width  - 1.0f) / P(0,0); // Should be mClientWidth and Height, instead of the Viewport's Width and Height.
		float vy = (-2.0f * newCursorScreenPos_clientSpace.m128_f32[1] / theViewport.Height + 1.0f) / P(1,1);

		// Ray definition in view space.
		rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		rayDir    = XMVectorSet(vx, vy, 1.0f, 0.0f);

		float distanceToIntersectionPoint;

		// Re-select the rotation sphere. This call should never return false in this function.
		bool selected = tryForSelection(rayOrigin, rayDir, theCamera.View(), distanceToIntersectionPoint);
		if(!selected)
		{
			throw "Error in Handle_RotationSphere::pickSphere(); Re-selection of rotation sphere failed during a 'spin-around' operation.";
		}
	}
}

/* Called when picking against the rotation sphere should cease, if the LMB has been released. */
void Handle_RotationSphere::unselect(XMVECTOR &pickingRay)
{
	isSelected = false;
}

/* Called to see if this is the currently selected rotation handle, if any. */
bool Handle_RotationSphere::getIsSelected()
{
	return isSelected;
}

/* Called to retrieve the total rotation quaternion. */
XMVECTOR Handle_RotationSphere::getTotalRotationQuaternion()
{
	return XMQuaternionMultiply(XMLoadFloat4(&quaternionRotationMadeSoFar), XMLoadFloat4(&lastQuaternionRotationMade)); // getLastRotationQuaternion());
}

/* Called to retrieve the last made rotation quaternion. */
void Handle_RotationSphere::calcLastRotationQuaternion()
{
	XMVECTOR loadedCurrentlyPickedPointOnSphere = XMLoadFloat3(&currentlyPickedPointOnSphere);
	XMVECTOR loadedFirstPickedPointOnSphere = XMLoadFloat3(&firstPickedPointOnSphere);

	XMVECTOR rotationAxis = calcRotationAxisFromTwoPointsOnSphere(sphere, loadedFirstPickedPointOnSphere, loadedCurrentlyPickedPointOnSphere);
	float rotationAngle = calcAngleBetweenTwoPointsOnSphere(sphere, loadedFirstPickedPointOnSphere, loadedCurrentlyPickedPointOnSphere); // MathHelper::Pi * 0.8432f is approx. as big as the rotation angle gets.

	BOOL rotationAxisIsAllZeroes = XMVector3Equal(rotationAxis, XMVectorZero());
	if(rotationAxisIsAllZeroes == TRUE) // This occurs when firstPickedPoint == currentlyPickedPoint (only then?), in which case an arbitrary rotation axis and a 0 radian "angle" is chosen.
	{
		rotationAxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		rotationAngle = 0.0f;
	}

	XMStoreFloat4(&lastQuaternionRotationMade, XMQuaternionRotationAxis(rotationAxis, -rotationAngle));
}

void Handle_RotationSphere::resetRotationQuaternion()
{
	quaternionRotationMadeSoFar = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
}

/* Called for the needed transform of the visual and/or bounding components of the handle. */
void Handle_RotationSphere::setWorld(XMMATRIX &world)
{
	XMStoreFloat4x4(&this->world, world);
}