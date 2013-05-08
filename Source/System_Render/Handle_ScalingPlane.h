#ifndef HANDLE_SCALINGPLANE_H
#define HANDLE_SCALINGPLANE_H

#include <Core/Math.h>

enum DimensionTuple {XY, XZ, YZ};

class Handle_ScalingPlane
{
private:
	XMFLOAT4X4 world;

	MyPlane plane;
	MyRectangle boundingRectangle;

	XMFLOAT3 prevPickedPointOnAxisPlane;
	XMFLOAT3 nextPickedPointOnAxisPlane;

	XMFLOAT3 firstPickedPointOnAxisPlane;
	XMFLOAT3 currentlyPickedPointOnAxisPlane;

	bool isSelected;

	HWND windowHandle;

	int up_or_down_CrossOverScaleFactor;
	int left_or_right_CrossOverScaleFactor;

	float currentScaleFactor;

	XMFLOAT3 totalScaling;

	DimensionTuple dimensionTuple;

	XMFLOAT3 lastScalingDeltaMade;
	XMFLOAT3 scalingDeltaMadeSoFar;

	bool shouldFlipMouseCursor;

public:
	Handle_ScalingPlane(XMVECTOR &normal, float offset, MyRectangle boundingRectangle, /*HWND windowHandle,*/ DimensionTuple dimensionTuple);
	~Handle_ScalingPlane();
	
	/* Called for initial selection and picking against the axis plane. */
	bool tryForSelection(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, float &distanceToIntersectionPoint);
	
	/* Called for continued picking against the plane. */
	void pickPlane(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, XMMATRIX &camProj, D3D11_VIEWPORT &theViewport);

	/* Called when picking against the plane should cease, and the last translation made final. */
	void unselect();
	
	/* Called to see if this is the currently selected translation axis, if any. */
	bool getIsSelected();

	/* Called to retrieve the last made translation delta. */
	void calcLastScalingDelta();

	/* Called to retrieve the last made translation delta. */
	XMVECTOR getTotalScalingDelta();

	/* Called for the needed transform of the visual and/or bounding components of the handle. */
	void setWorld(XMMATRIX &world);

	void resetScalingDelta();

	void setShouldFlipMouseCursor(bool shouldFlipMouseCursor);

	// Move somewhere else.
	bool rayVsRectangle(XMVECTOR &rayOrigin, XMVECTOR &rayDir, MyRectangle &rectangle, float &distanceToIntersectionPoint);
};

#endif