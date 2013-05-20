#ifndef HANDLE_ROTATIONPLANE_H
#define HANDLE_ROTATIONPLANE_H

#include <Core/Math.h>

#include "IHandle.h"
#include "xnacollision.h"

using XNA::Sphere;

class Handle_RotationPlane
{
private:
	XMFLOAT4 centerPoint;

	XMFLOAT4X4 world;
		
	MyPlane plane;
	MyRectangle boundingRectangle;

	Sphere sphere;

	XMFLOAT3 prevPickedPointOnAxisPlane;
	XMFLOAT3 nextPickedPointOnAxisPlane;

	XMFLOAT3 firstPickedPointOnAxisPlane;
	XMFLOAT3 currentlyPickedPointOnAxisPlane;

	bool isSelected;

public:
	Handle_RotationPlane(XMVECTOR &direction, std::vector<XMFLOAT4> boundingLines, char axis);
	~Handle_RotationPlane();
	
	/* Called for initial selection and picking against the axis plane. */
	bool tryForSelection(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, float &distanceToIntersectionPoint);

	/* Called for initial selection and picking against the axis plane. */
	bool pickFirstPointOnPlane(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, float &distanceToIntersectionPoint);
	
	/* Called for continued picking against the plane. */
	void pickPlane(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView);

	/* Called when picking against the plane should cease, and the last translation made final. */
	void unselect();
	
	/* Called to see if this is the currently selected translation axis, if any. */
	bool getIsSelected();

	/* Called to retrieve the last made translation delta. */
	XMVECTOR getCurrentRotationQuaternion();

	/* Called for the needed transform of the visual and/or bounding components of the handle. */
	void setWorld(XMMATRIX &world);

	/* Called to set the plane orientation. Used for single-axis translation, by axis-specific handles relying on translation planes. */
	void setPlaneOrientation(XMVECTOR &normal);

	// Move somewhere else.
	bool rayVsRectangle(XMVECTOR &rayOrigin, XMVECTOR &rayDir, MyRectangle &rectangle, float &distanceToIntersectionPoint);
};

#endif