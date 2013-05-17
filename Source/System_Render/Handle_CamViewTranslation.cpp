//#include "stdafx.h"
//#include "Handle_OmniDimensionalScaling.h"
//
//Handle_CamViewTranslation::Handle_CamViewTranslation(std::vector<XMFLOAT4> boundingTriangles)
//{
//	isSelected = false;
//
//	this->boundingTriangles = boundingTriangles;
//
//	MyRectangle boundingRectangle; // Dummy rectangle.
//	camViewPlane = new Handle_CamViewPlane();
//
//	camViewPlane->setShouldFlipCursorAroundSceneView(true);
//}
//
//Handle_OmniDimensionalScaling::~Handle_OmniDimensionalScaling()
//{
//	delete camViewPlane;
//}
//
//Handle_OmniDimensionalScaling::rayVsRectangle(XMVECTOR &rayOrigin, XMVECTOR &rayDir, MyRectangle &rectangle, float &distanceToIntersectionPoint)
//{
//	bool rayIntersectedWithRectangle = false;
//
//	// Update the bounding rectangle... (Currently not used.)
//	//XMVECTOR transformedP1 = XMVector3Transform(XMLoadFloat3(&rectangle.P1), XMLoadFloat4x4(&world));
//	//XMVECTOR transformedP2 = XMVector3Transform(XMLoadFloat3(&rectangle.P2), XMLoadFloat4x4(&world));
//	//XMVECTOR transformedP3 = XMVector3Transform(XMLoadFloat3(&rectangle.P3), XMLoadFloat4x4(&world));
//	//XMVECTOR transformedP4 = XMVector3Transform(XMLoadFloat3(&rectangle.P4), XMLoadFloat4x4(&world));
//
//	// Calculate the definition of the plane that the rectangle is lying in.
//	XMVECTOR planeVector = XMPlaneFromPoints(XMLoadFloat3(&rectangle.P1), XMLoadFloat3(&rectangle.P2), XMLoadFloat3(&rectangle.P3));
//
//	MyPlane plane;
//	plane.normal.x = planeVector.m128_f32[0];
//	plane.normal.y = planeVector.m128_f32[1];
//	plane.normal.z = planeVector.m128_f32[2];
//	plane.offset = planeVector.m128_f32[3];
//
//	XMVECTOR pointOfIntersection;
//	rayVsPlane(rayOrigin, rayDir, plane, pointOfIntersection);
//
//	//XMVECTOR transformedIntersectionPoint = XMVector3Transform(pointOfIntersection, XMLoadFloat4x4(&world));
//
//	XMVECTOR loadedP1 = XMLoadFloat3(&rectangle.P1); //transformedP1;
//	XMVECTOR loadedP2 = XMLoadFloat3(&rectangle.P2); //transformedP2;
//	XMVECTOR loadedP3 = XMLoadFloat3(&rectangle.P3); //transformedP3;
//	XMVECTOR loadedP4 = XMLoadFloat3(&rectangle.P4); //transformedP4;
//
//	XMVECTOR V1 = loadedP2 - loadedP1;
//	XMVECTOR V3 = loadedP4 - loadedP3;
//	XMVECTOR V4 = pointOfIntersection - loadedP1;
//	XMVECTOR V5 = pointOfIntersection - loadedP3;
//
//	XMVECTOR V6 = loadedP2 - loadedP3;
//	XMVECTOR V7 = loadedP4 - loadedP1;
//	XMVECTOR V8 = pointOfIntersection - loadedP3;
//	XMVECTOR V9 = pointOfIntersection - loadedP1;
//
//	V1 = XMVector3Normalize(V1);
//	V3 = XMVector3Normalize(V3);
//	V4 = XMVector3Normalize(V4);
//	V5 = XMVector3Normalize(V5);
//	V6 = XMVector3Normalize(V6);
//	V7 = XMVector3Normalize(V7);
//	V8 = XMVector3Normalize(V8);
//	V9 = XMVector3Normalize(V9);
//
//	XMVECTOR V1dotV4 = XMVector3Dot(V1, V4);
//	XMVECTOR V3dotV5 = XMVector3Dot(V3, V5);
//	XMVECTOR V6dotV8 = XMVector3Dot(V6, V8);
//	XMVECTOR V7dotV9 = XMVector3Dot(V7, V9);
//
//	if(		V1dotV4.m128_f32[0] >= 0.0f && V3dotV5.m128_f32[0] >= 0.0f
//		&&	V6dotV8.m128_f32[0] >= 0.0f && V7dotV9.m128_f32[0] >= 0.0f	)
//	{
//		rayIntersectedWithRectangle = true;
//		distanceToIntersectionPoint = XMVector3Length(rayOrigin - pointOfIntersection).m128_f32[0];
//	}
//
//	return rayIntersectedWithRectangle;
//}
//
///* Called for initial selection and picking against the axis plane. */
//bool Handle_OmniDimensionalScaling::tryForSelection(MyRectangle &selectionRectangle, XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, float &distanceToIntersectionPoint, POINT &mouseCursorPoint)
//{
//	bool wasSelected = false;
//
//	// Tranform ray to local space.
//	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(camView), camView);
//
//	XMMATRIX W = XMLoadFloat4x4(&world);
//	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);
//
//	XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);
//
//	XMVECTOR transRayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
//	XMVECTOR transRayDir = XMVector3TransformNormal(rayDir, toLocal);
//
//	// Make the ray direction unit length for the intersection tests.
//	transRayDir = XMVector3Normalize(transRayDir);
//
//	float dist;
//	for(unsigned int i = 0; i < boundingTriangles.size(); i = i + 3)
//	{
//		wasSelected = XNA::IntersectRayTriangle(transRayOrigin, transRayDir, XMLoadFloat4(&boundingTriangles.at(i)), XMLoadFloat4(&boundingTriangles.at(i + 1)), XMLoadFloat4(&boundingTriangles.at(i + 2)), &dist);
//
//		if(wasSelected)
//		{
//			XMVECTOR normalizedRayDir = XMVector3Normalize(rayDir);
//
//			camViewPlane->setFirstPointOnViewPlane(mouseCursorPoint);
//
//			distanceToIntersectionPoint = dist;
//			break;
//		}
//	}
//
//	isSelected = wasSelected;
//
//	return wasSelected;
//}
//
///* Called when picking against the axis plane should cease, if the LMB has been released. */
//void Handle_OmniDimensionalScaling::unselect()
//{
//	isSelected = false;
//	camViewPlane->unselect();
//}
//	
///* Called to see if this is the currently selected translation axis, if any. */
//bool Handle_OmniDimensionalScaling::getIsSelected()
//{
//	return isSelected;
//}
//
//void Handle_OmniDimensionalScaling::calcLastScalingDelta()
//{
//	//camViewPlane->calcLastScalingDelta();
//}
//
//POINT Handle_OmniDimensionalScaling::getTotalScalingDelta()
//{
//	POINT totalScalingXYDeltas;
//	totalScalingXYDeltas.x = 0;
//	totalScalingXYDeltas.y = 0;
//
//	if(camViewPlane->getIsSelected())
//	{
//		totalScalingXYDeltas = camViewPlane->getTotalMouseCursorXYDeltas();
//	}
//
//	return totalScalingXYDeltas;
//}
//
///* Called for the needed transform of the visual and/or bounding components of the handle. */
//void Handle_OmniDimensionalScaling::setWorld(XMMATRIX &world)
//{
//	XMStoreFloat4x4(&this->world, world);
//}
//
//void Handle_OmniDimensionalScaling::update(POINT &mouseCursorPoint)
//{
//	camViewPlane->update(mouseCursorPoint);
//
//	//// Tranform ray to local space.
//	//XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(camView), camView);
//
//	//XMMATRIX W = XMLoadFloat4x4(&world);
//	//XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);
//
//	//XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);
//
//	//XMVECTOR transRayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
//	//XMVECTOR transRayDir = XMVector3TransformNormal(rayDir, toLocal);
//
//	//// Make the ray direction unit length for the intersection tests.
//	//transRayDir = XMVector3Normalize(transRayDir);
//}