#ifndef HANDLE_CAM_VIEW_PLANE
#define HANDLE_CAM_VIEW_PLANE

#include "IHandle.h"

class Handle_CamViewPlane : public IHandle
{
private: 
	POINT firstPickedMousePoint;
	POINT currentlyPickedMousePoint;

	int currentlyPickedMousePointX;
	int currentlyPickedMousePointY;

	POINT mouseXYDeltasMadeSoFar;
	int mouseXDeltasMadeSoFar; // For accounting when "flipping" the mouse cursor.
	int mouseYDeltasMadeSoFar;

	bool isSelected;
	bool shouldFlipCursorAroundSceneView;

public:
	Handle_CamViewPlane();
	~Handle_CamViewPlane();

	bool getIsSelected();
	void unselect();

	void setShouldFlipCursorAroundSceneView(bool shouldFlipCursor);
	void setFirstPointOnViewPlane(POINT mouseCursorPoint);
	void update(POINT currentMouseCursorPoint);
	POINT getTotalMouseCursorXYDeltas();
};

#endif