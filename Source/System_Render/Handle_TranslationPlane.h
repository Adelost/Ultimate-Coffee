#ifndef HANDLE_TRANSLATIONPLANE_H
#define HANDLE_TRANSLATIONPLANE_H

#include <Core/Math.h>

class Handle_TranslationPlane
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

public:
	Handle_TranslationPlane(XMVECTOR &normal, float offset, MyRectangle boundingRectangle);
	~Handle_TranslationPlane();
	
	/* Called for initial selection and picking against the axis plane. */
	bool tryForSelection(MyRectangle &selectionRectangle, XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, float &distanceToIntersectionPoint);

	/* Called for initial selection and picking against the axis plane. */
	bool pickFirstPointOnPlane(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, float &distanceToIntersectionPoint);
	
	/* Called for continued picking against the plane. */
	void pickPlane(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView);

	/* Called when picking against the plane should cease, and the last translation made final. */
	void unselect();
	
	/* Called to see if this is the currently selected translation axis, if any. */
	bool getIsSelected();

	/* Called to retrieve the last made translation delta. */
	XMVECTOR getLastTranslationDelta();

	/* Called for the needed transform of the visual and/or bounding components of the handle. */
	void setWorld(XMMATRIX &world);

	/* Called to set the plane orientation. Used for single-axis translation, by axis-specific handles relying on translation planes. */
	void setPlaneOrientation(XMVECTOR &normal);

	// Move somewhere else.
	bool rayVsRectangle(XMVECTOR &rayOrigin, XMVECTOR &rayDir, MyRectangle &rectangle, float &distanceToIntersectionPoint);
};

#endif