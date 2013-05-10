#include "stdafx.h"
#include "Handle_ScalingPlane.h"

#include <Core/Events.h>

Handle_ScalingPlane::Handle_ScalingPlane(XMVECTOR &normal, float offset, MyRectangle boundingRectangle, /*HWND windowHandle,*/ DimensionTuple dimensionTuple)
{
	XMStoreFloat3(&plane.normal, normal);
	plane.offset = offset;
	this->boundingRectangle = boundingRectangle;

	this->windowHandle = windowHandle;

	up_or_down_CrossOverScaleFactor = 0;
	left_or_right_CrossOverScaleFactor = 0;

	this->dimensionTuple = dimensionTuple;

	isSelected = false;

	lastScalingDeltaMade = XMFLOAT3(0.0f, 0.0f, 0.0f);
	scalingDeltaMadeSoFar = XMFLOAT3(0.0f, 0.0f, 0.0f);

	shouldFlipMouseCursor = false;
}

Handle_ScalingPlane::~Handle_ScalingPlane()
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

bool Handle_ScalingPlane::rayVsRectangle(XMVECTOR &rayOrigin, XMVECTOR &rayDir, MyRectangle &rectangle, float &distanceToIntersectionPoint)
{
	bool rayIntersectedWithRectangle = false;

	// Update the bounding rectangle... (Currently not used.)
	//XMVECTOR transformedP1 = XMVector3Transform(XMLoadFloat3(&rectangle.P1), XMLoadFloat4x4(&world));
	//XMVECTOR transformedP2 = XMVector3Transform(XMLoadFloat3(&rectangle.P2), XMLoadFloat4x4(&world));
	//XMVECTOR transformedP3 = XMVector3Transform(XMLoadFloat3(&rectangle.P3), XMLoadFloat4x4(&world));
	//XMVECTOR transformedP4 = XMVector3Transform(XMLoadFloat3(&rectangle.P4), XMLoadFloat4x4(&world));

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

	XMVECTOR loadedP1 = XMLoadFloat3(&rectangle.P1); //transformedP1;
	XMVECTOR loadedP2 = XMLoadFloat3(&rectangle.P2); //transformedP2;
	XMVECTOR loadedP3 = XMLoadFloat3(&rectangle.P3); //transformedP3;
	XMVECTOR loadedP4 = XMLoadFloat3(&rectangle.P4); //transformedP4;

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
bool Handle_ScalingPlane::tryForSelection(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, float &distanceToIntersectionPoint)
{
	// TEST 
	int a = 5;


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
	
/* Called for continued picking against the axis plane, if LMB has yet to be released. */
void Handle_ScalingPlane::pickPlane(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, XMMATRIX &camProj, D3D11_VIEWPORT &theViewport)
{
	//prevPickedPointOnAxisPlane = nextPickedPointOnAxisPlane;

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

	if(!shouldFlipMouseCursor)
	{
		XMStoreFloat3(&currentlyPickedPointOnAxisPlane, planeIntersectionPoint);
	}
	else
	{
		//XMVECTOR planeIntersectionPoint;
		float distanceToIntersectionPoint;
		bool rayIntersectedRectangle = rayVsRectangle(transRayOrigin, transRayDir, boundingRectangle, distanceToIntersectionPoint);

		// If the rectangle was missed, "flip" the mouse to the opposite point of the rectangle.
		if(rayIntersectedRectangle)
		{
			XMStoreFloat3(&currentlyPickedPointOnAxisPlane, transRayOrigin + transRayDir * distanceToIntersectionPoint);
		}
		else
		{
			/*	Since the rectangle has been selected and the mouse pointer has since been moved,
				missing the rectangle now would mean that the cursor has moved past its boundary,
				and, so, the mouse pointer is to be "flipped around" to the opposite boundary point.	*/

			// Take the scaling delta made before the coming "flip-around" into account.
			XMStoreFloat3(&scalingDeltaMadeSoFar, XMLoadFloat3(&scalingDeltaMadeSoFar) +  XMLoadFloat3(&lastScalingDeltaMade));

			float xMiddle, yMiddle, zMiddle;
			switch (dimensionTuple)
			{
				case XY:
					xMiddle = 0.5f;
					yMiddle = 0.5f;
					zMiddle = 0.0f;
					break;
				case XZ:
					xMiddle = 0.5f;
					yMiddle = 0.0f;
					zMiddle = 0.5f;
					break;
				case YZ:
					xMiddle = 0.0f;
					yMiddle = 0.5f;
					zMiddle = 0.5f;
					break;
			}

			XMVECTOR rectangleCenterPoint = XMVectorSet(xMiddle, yMiddle, zMiddle, 0.0f);
			XMVECTOR exitPointVector = XMLoadFloat3(&currentlyPickedPointOnAxisPlane) - rectangleCenterPoint; // - XMLoadFloat3(&sphere.Center);
			XMVECTOR reEntryPointVector = (rectangleCenterPoint - exitPointVector * 0.95f) ;

			// Increase/decrease scale factor going by what side of the scaling rectangle was crossed...

			if(planeIntersectionPoint.m128_f32[0] >= this->boundingRectangle.P4.x)
			{
				++left_or_right_CrossOverScaleFactor; // The "right" side was crossed.
			}
		
			if(planeIntersectionPoint.m128_f32[0] <= this->boundingRectangle.P1.x)
			{
				--left_or_right_CrossOverScaleFactor; // The "left" side was crossed.
			}

			if(planeIntersectionPoint.m128_f32[1] >= this->boundingRectangle.P2.y)
			{
				++up_or_down_CrossOverScaleFactor; // The "top" side was crossed.
			}

			if(planeIntersectionPoint.m128_f32[1] <= this->boundingRectangle.P1.y)
			{
				--up_or_down_CrossOverScaleFactor; // The "bottom" side was crossed.
			}
		
			int viewPortHeight = SETTINGS()->windowSize.y;
			int viewPortWidth = SETTINGS()->windowSize.x;

			int DXViewPortTopLeftX = SETTINGS()->DXViewPortTopLeftX;
			int DXViewPortTopLeftY = SETTINGS()->DXViewPortTopLeftY;
			int DXViewPortMinDepth = SETTINGS()->DXViewPortMinDepth;
			int DXViewPortMaxDepth = SETTINGS()->DXViewPortMaxDepth;

			// Project the 3D re-entry point to 2D, so that the screen coordinates can be calculated.
			XMVECTOR newCursorScreenPos_clientSpace = XMVector3Project(	reEntryPointVector,
																		DXViewPortTopLeftX,	DXViewPortTopLeftY,
																		viewPortWidth,		viewPortHeight,
																		DXViewPortMinDepth,	DXViewPortMaxDepth,
																		camProj, camView, XMLoadFloat4x4(&world)	);

			Int2 newQtCursorPoint;
			newQtCursorPoint.x = newCursorScreenPos_clientSpace.m128_f32[0];
			newQtCursorPoint.y = newCursorScreenPos_clientSpace.m128_f32[1];
			SEND_EVENT(&Event_SetCursorPosition(newQtCursorPoint));

							//// Put the client-space coordinates in a POINT struct and convert it to screen-space coordinates...
							//POINT newMouseCursorPoint;
							//newMouseCursorPoint.x = newCursorScreenPos_clientSpace.m128_f32[0];
							//newMouseCursorPoint.y = newCursorScreenPos_clientSpace.m128_f32[1];
							//ClientToScreen(windowHandle, &newMouseCursorPoint);

							//// ... then set the mouse cursor's position to the result (x, y) coordinates.
							//SetCursorPos(newMouseCursorPoint.x, newMouseCursorPoint.y);

			// Since the cursor position has changed, the picking ray/origin, is updated below...

			XMMATRIX P = camProj;

			// Compute picking ray in view space, using the client-space coordinates.
			float vx = (+2.0f * newCursorScreenPos_clientSpace.m128_f32[0] / viewPortWidth  - 1.0f) / P.r[0].m128_f32[0]; // Should be mClientWidth and Height, instead of the Viewport's Width and Height.
			float vy = (-2.0f * newCursorScreenPos_clientSpace.m128_f32[1] / viewPortHeight + 1.0f) / P.r[1].m128_f32[1];

			// Ray definition in view space.
			rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			rayDir    = XMVectorSet(vx, vy, 1.0f, 0.0f);

			float distanceToIntersectionPoint;

			// Re-select the rotation sphere. This call should never return false in this function.
			bool selected = tryForSelection(rayOrigin, rayDir, camView, distanceToIntersectionPoint);
			if(!selected)
			{
				throw "Error in Handle_ScalingPlane::pickPlane(); Re-selection of scaling rectangle failed during a 'flip-around' operation.";
			}
		}
	}
}

/* Called when picking against the axis plane should cease, if the LMB has been released. */
void Handle_ScalingPlane::unselect()
{
	firstPickedPointOnAxisPlane.x = 0.0f;
	firstPickedPointOnAxisPlane.y = 0.0f;
	firstPickedPointOnAxisPlane.z = 0.0f;
	currentlyPickedPointOnAxisPlane.x = 0.0f;
	currentlyPickedPointOnAxisPlane.y = 0.0f;
	currentlyPickedPointOnAxisPlane.z = 0.0f;

	up_or_down_CrossOverScaleFactor = 0;
	left_or_right_CrossOverScaleFactor = 0;

	isSelected = false;
}
	
/* Called to see if this is the currently selected translation plane, if any. */
bool Handle_ScalingPlane::getIsSelected()
{
	return isSelected;
}

/* Called to retrieve the last made translation delta. */
void Handle_ScalingPlane::calcLastScalingDelta()
{
	//XMVECTOR deltaVector = nextPickedPointOnAxisPlane - prevPickedPointOnAxisPlane;

	XMVECTOR loadedCurrentlyPickedPointOnAxisPlane = XMLoadFloat3(&currentlyPickedPointOnAxisPlane);
	XMVECTOR loadedFirstPickedPointOnAxisPlane = XMLoadFloat3(&firstPickedPointOnAxisPlane);

	XMVECTOR currentScalingDelta = loadedCurrentlyPickedPointOnAxisPlane - loadedFirstPickedPointOnAxisPlane;

	if(shouldFlipMouseCursor)
	{
		XMVECTOR rectangleSideCrossOverFactors;
		switch (dimensionTuple)
		{
			case XY:
				rectangleSideCrossOverFactors.m128_f32[0] = abs(left_or_right_CrossOverScaleFactor) * 1.125;
				rectangleSideCrossOverFactors.m128_f32[1] = abs(up_or_down_CrossOverScaleFactor) * 1.125;
				rectangleSideCrossOverFactors.m128_f32[2] = 1.0f;
				break;
			case XZ:
				rectangleSideCrossOverFactors.m128_f32[0] = abs(left_or_right_CrossOverScaleFactor) * 1.125;
				rectangleSideCrossOverFactors.m128_f32[1] = 1.0f;
				rectangleSideCrossOverFactors.m128_f32[2] = abs(up_or_down_CrossOverScaleFactor) * 1.125;
				break;
			case YZ:
				rectangleSideCrossOverFactors.m128_f32[0] = 1.0f;
				rectangleSideCrossOverFactors.m128_f32[1] = abs(left_or_right_CrossOverScaleFactor) * 1.125;
				rectangleSideCrossOverFactors.m128_f32[2] = abs(up_or_down_CrossOverScaleFactor) * 1.125;
				break;
		}
		if(rectangleSideCrossOverFactors.m128_f32[0] == 0.0f)
			rectangleSideCrossOverFactors.m128_f32[0] = 1.0f;
		if(rectangleSideCrossOverFactors.m128_f32[1] == 0.0f)
			rectangleSideCrossOverFactors.m128_f32[1] = 1.0f;
		if(rectangleSideCrossOverFactors.m128_f32[2] = 0.0f)
			rectangleSideCrossOverFactors.m128_f32[2] == 1.0f;

		currentScalingDelta *= rectangleSideCrossOverFactors;
	}

	XMStoreFloat3(&lastScalingDeltaMade, currentScalingDelta);
}

/* Called to retrieve the last made translation delta. */
XMVECTOR Handle_ScalingPlane::getTotalScalingDelta()
{
	return XMLoadFloat3(&scalingDeltaMadeSoFar) + XMLoadFloat3(&lastScalingDeltaMade);
}

/* Called for the needed transform of the visual and/or bounding components of the handle. */
void Handle_ScalingPlane::setWorld(XMMATRIX &world)
{
	XMStoreFloat4x4(&this->world, world);
}

void Handle_ScalingPlane::resetScalingDelta()
{
	lastScalingDeltaMade = XMFLOAT3(0.0f, 0.0f, 0.0f);
	scalingDeltaMadeSoFar = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void Handle_ScalingPlane::setShouldFlipMouseCursor(bool shouldFlipMouseCursor)
{
	this->shouldFlipMouseCursor = shouldFlipMouseCursor;
}














		//// Upper right quadrant.
		//if(exitFromOrigo.m128_f32[0] >= rectangleCenterPoint.m128_f32[0] && exitPointVector.m128_f32[1] - rectangleCenterPoint.m128_f32[1] >= rectangleCenterPoint.m128_f32[1])
		//{
		//	if(abs(exitFromOrigo.m128_f32[0] >= abs(exitPointVector.m128_f32[1] - rectangleCenterPoint.m128_f32[1]))
		//	{
		//		// Upper upper right quadrant.
		//		++left_or_right_CrossOverScaleFactor;
		//	}
		//	else
		//		++up_or_down_CrossOverScaleFactor;
		//}
		//// Upper left quadrant.
		//else if(exitPointVector.m128_f32[0] - rectangleCenterPoint.m128_f32[0] <= rectangleCenterPoint.m128_f32[0] && exitPointVector.m128_f32[1] - rectangleCenterPoint.m128_f32[1] >= rectangleCenterPoint.m128_f32[1])
		//{
		//	if(abs(exitPointVector.m128_f32[0] - rectangleCenterPoint.m128_f32[0]) <= abs(exitPointVector.m128_f32[1] - rectangleCenterPoint.m128_f32[1]))
		//	{
		//		// Upper upper right quadrant.
		//		--left_or_right_CrossOverScaleFactor;
		//	}
		//	else
		//		++up_or_down_CrossOverScaleFactor;
		//}
		//// Lower left quadrant.
		//else if(exitPointVector.m128_f32[0] - rectangleCenterPoint.m128_f32[0] <= rectangleCenterPoint.m128_f32[0] && exitPointVector.m128_f32[1] - rectangleCenterPoint.m128_f32[1] <= rectangleCenterPoint.m128_f32[1])
		//{
		//	if(abs(exitPointVector.m128_f32[0] - rectangleCenterPoint.m128_f32[0]) <= abs(exitPointVector.m128_f32[1] - rectangleCenterPoint.m128_f32[1]))
		//	{
		//		// Upper upper right quadrant.
		//		--left_or_right_CrossOverScaleFactor;
		//	}
		//	else
		//		--up_or_down_CrossOverScaleFactor;
		//}
		//// Lower right quadrant.
		//else if(exitPointVector.m128_f32[0] - rectangleCenterPoint.m128_f32[0] >= rectangleCenterPoint.m128_f32[0] && exitPointVector.m128_f32[1] - rectangleCenterPoint.m128_f32[1] <= rectangleCenterPoint.m128_f32[1])
		//{
		//	if(abs(exitPointVector.m128_f32[0] - rectangleCenterPoint.m128_f32[0]) >= abs(exitPointVector.m128_f32[1] - rectangleCenterPoint.m128_f32[1]))
		//	{
		//		// Upper upper right quadrant.
		//		++left_or_right_CrossOverScaleFactor;
		//	}
		//	else
		//		--up_or_down_CrossOverScaleFactor;
		//}

		//// Upper right quadrant.
		//if(exitPointVector.m128_f32[0] - rectangleCenterPoint.m128_f32[0] >= rectangleCenterPoint.m128_f32[0] && exitPointVector.m128_f32[1] - rectangleCenterPoint.m128_f32[1] >= rectangleCenterPoint.m128_f32[1])
		//{
		//	if(abs(exitPointVector.m128_f32[0] - rectangleCenterPoint.m128_f32[0]) >= abs(exitPointVector.m128_f32[1] - rectangleCenterPoint.m128_f32[1]))
		//	{
		//		// Upper upper right quadrant.
		//		++left_or_right_CrossOverScaleFactor;
		//	}
		//	else
		//		++up_or_down_CrossOverScaleFactor;
		//}
		//// Upper left quadrant.
		//else if(exitPointVector.m128_f32[0] - rectangleCenterPoint.m128_f32[0] <= rectangleCenterPoint.m128_f32[0] && exitPointVector.m128_f32[1] - rectangleCenterPoint.m128_f32[1] >= rectangleCenterPoint.m128_f32[1])
		//{
		//	if(abs(exitPointVector.m128_f32[0] - rectangleCenterPoint.m128_f32[0]) <= abs(exitPointVector.m128_f32[1] - rectangleCenterPoint.m128_f32[1]))
		//	{
		//		// Upper upper right quadrant.
		//		--left_or_right_CrossOverScaleFactor;
		//	}
		//	else
		//		++up_or_down_CrossOverScaleFactor;
		//}
		//// Lower left quadrant.
		//else if(exitPointVector.m128_f32[0] - rectangleCenterPoint.m128_f32[0] <= rectangleCenterPoint.m128_f32[0] && exitPointVector.m128_f32[1] - rectangleCenterPoint.m128_f32[1] <= rectangleCenterPoint.m128_f32[1])
		//{
		//	if(abs(exitPointVector.m128_f32[0] - rectangleCenterPoint.m128_f32[0]) <= abs(exitPointVector.m128_f32[1] - rectangleCenterPoint.m128_f32[1]))
		//	{
		//		// Upper upper right quadrant.
		//		--left_or_right_CrossOverScaleFactor;
		//	}
		//	else
		//		--up_or_down_CrossOverScaleFactor;
		//}
		//// Lower right quadrant.
		//else if(exitPointVector.m128_f32[0] - rectangleCenterPoint.m128_f32[0] >= rectangleCenterPoint.m128_f32[0] && exitPointVector.m128_f32[1] - rectangleCenterPoint.m128_f32[1] <= rectangleCenterPoint.m128_f32[1])
		//{
		//	if(abs(exitPointVector.m128_f32[0] - rectangleCenterPoint.m128_f32[0]) >= abs(exitPointVector.m128_f32[1] - rectangleCenterPoint.m128_f32[1]))
		//	{
		//		// Upper upper right quadrant.
		//		++left_or_right_CrossOverScaleFactor;
		//	}
		//	else
		//		--up_or_down_CrossOverScaleFactor;
		//}


		//if(exitPointVector.m128_f32[0] > rectangleCenterPoint.m128_f32[0])
		//{
		//	if(exitPointVector.m128_f32[0] > exitPointVector.m128_f32[1] - rectangleCenterPoint.m128_f32[0])
		//	{
		//		++left_or_right_CrossOverScaleFactor;
		//	}
		//	else if(exitPointVector.m128_f32[1] > rectangleCenterPoint.m128_f32[1] && exitPointVector.m128_f32[1] > exitPointVector.m128_f32[0])
		//	{
		//		++up_or_down_CrossOverScaleFactor;
		//	}
		//	else
		//		--up_or_down_CrossOverScaleFactor;
		//}
		//else if(exitPointVector.m128_f32[0] < rectangleCenterPoint.m128_f32[0])
		//{
		//	if(exitPointVector.m128_f32[0] > exitPointVector.m128_f32[1] - rectangleCenterPoint.m128_f32[0])
		//	{
		//		--left_or_right_CrossOverScaleFactor;
		//	}
		//	else if(exitPointVector.m128_f32[1] > rectangleCenterPoint.m128_f32[1] && exitPointVector.m128_f32[1] > exitPointVector.m128_f32[0])
		//	{
		//		++up_or_down_CrossOverScaleFactor;
		//	}
		//	else
		//		--up_or_down_CrossOverScaleFactor;
		//}

		//if(reEntryPointVector.m128_f32[0] < reEntryPointVector.m128_f32[1])
		//{
		//	if(exitPointVector.m128_f32[0] < 0.0f)
		//		--left_or_right_CrossOverScaleFactor;
		//	else
		//		++left_or_right_CrossOverScaleFactor;
		//}
		//else if(reEntryPointVector.m128_f32[1] < reEntryPointVector.m128_f32[0])
		//{
		//	if(exitPointVector.m128_f32[0] < 0.0f)
		//		--up_or_down_CrossOverScaleFactor;
		//	else
		//		++up_or_down_CrossOverScaleFactor;
		//}