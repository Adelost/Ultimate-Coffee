#ifndef HANDLE_OMNI_DIMENSIONAL_SCALING
#define HANDLE_OMNI_DIMENSIONAL_SCALING

#include "stdafx.h"
#include "IHandle.h"
#include "Handle_CamViewPlane.h"

//#include "DirectXMath.h"
#include "XNACollision.h"

class Handle_OmniDimensionalScaling : public IHandle
{
private:
	Handle_CamViewPlane *camViewPlane;

	XMFLOAT4X4 world;

	//MyPlane plane;
	//MyRectangle boundingRectangle;

	//XMFLOAT3 prevPickedPointOnAxisPlane;
	//XMFLOAT3 nextPickedPointOnAxisPlane;

	//XMFLOAT3 firstPickedPointOnAxisPlane;
	//XMFLOAT3 currentlyPickedPointOnAxisPlane;

	bool isSelected;

	//HWND windowHandle;

	int up_or_down_CrossOverScaleFactor;
	int left_or_right_CrossOverScaleFactor;

	//float currentScaleFactor;

	//XMFLOAT3 totalScaling;

	//DimensionTuple dimensionTuple;

	//XMFLOAT3 lastScalingDeltaMade;
	//XMFLOAT3 scalingDeltaMadeSoFar;

	bool shouldFlipMouseCursor;

	std::vector<XMFLOAT4> boundingTriangles;

public:
	Handle_OmniDimensionalScaling(std::vector<XMFLOAT4> boundingTriangles);
	~Handle_OmniDimensionalScaling();
	
	/* Called for initial selection and picking against the axis plane. */
	bool tryForSelection(MyRectangle &selectionRectangle, XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, float &distanceToIntersectionPoint, POINT &mouseCursorPoint);
	
	//bool pickFirstPointOnViewPlane(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, float &distanceToPointOfIntersection);

	/* Called for continued picking against the axis plane. */
	//void pickViewPlane(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, XMMATRIX &camProj);

	/* Called when picking against the axis plane should cease, and the last translation made final. */
	void unselect();
	
	/* Called to see if this is the currently selected translation axis, if any. */
	bool getIsSelected();

	void calcLastScalingDelta();

	POINT getTotalScalingDelta();

	///* Called to retrieve the last made translation delta. */
	//POINT getLastTranslationDelta();

	/* Called for the needed transform of the visual and/or bounding components of the handle. */
	void setWorld(XMMATRIX &world);

	void update(POINT &mouseCursorPoint);
};

#endif