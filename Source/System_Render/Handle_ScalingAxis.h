#ifndef HANDLE_SCALINGAXIS_H
#define HANDLE_SCALINGAXIS_H

#include "Handle_ScalingPlane.h"

#include "DirectXMath.h"
#include "XNACollision.h"

using namespace DirectX;

class Handle_ScalingAxis
{
private:
	XMFLOAT4X4 world;
	XMFLOAT3 direction; // Should be X, Y, or Z.

	Handle_ScalingPlane *singleAxisScalingPlane;

	// The delta between the first and last picked point on the axis plane gives the current translation from the active object's last set position.
	XMFLOAT3 prevPickedPointOnAxisPlane;
	XMFLOAT3 nextPickedPointOnAxisPlane;

	std::vector<XMFLOAT4> boundingTriangles;

	bool isSelected;

public:
	Handle_ScalingAxis(XMVECTOR &direction, std::vector<XMFLOAT4> boundingTriangles, char axis);
	~Handle_ScalingAxis();
	
	/* Called for initial selection and picking against the axis plane. */
	bool tryForSelection(MyRectangle &selectionRectangle, XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, float &distanceToIntersectionPoint);
	
	bool pickFirstPointOnAxisPlane(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, float &distanceToPointOfIntersection);

	/* Called for continued picking against the axis plane. */
	void pickAxisPlane(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, XMMATRIX &camProj);

	/* Called when picking against the axis plane should cease, and the last translation made final. */
	void unselect();
	
	/* Called to see if this is the currently selected translation axis, if any. */
	bool getIsSelected();

	void calcLastScalingDelta();

	XMVECTOR getTotalScalingDelta();

	/* Called to retrieve the last made translation delta. */
	XMVECTOR getLastTranslationDelta();

	/* Called for the needed transform of the visual and/or bounding components of the handle. */
	void setWorld(XMMATRIX &world);

	void update(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView);
};

#endif