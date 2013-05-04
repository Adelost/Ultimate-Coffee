#include "stdafx.h"
#include "Core/Math.h"
#include "Tool_Selection.h"

Tool_Selection::Tool_Selection()
{
	isSelected = false;
}

Tool_Selection::~Tool_Selection()
{
}

void Tool_Selection::setIsVisible(bool &isVisible)
{
	this->isVisible = isVisible;
}

/* Called for an instance of picking, possibly resulting in the tool being selected. */
void Tool_Selection::beginSelection( XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, D3D11_VIEWPORT &theViewport, POINT &mouseCursorPoint, ITool_Transformation *currentlyChosenTransformationTool )
{
	isSelected = true;

	objectsThatHaveBeenSelected.clear();

	firstMouseCursorPoint = mouseCursorPoint;
	currentMouseCursorPoint.x = firstMouseCursorPoint.x + 4;
	currentMouseCursorPoint.y = firstMouseCursorPoint.y + 4;

	// The currently chosen transformation tool has an active object, then it is to be visible and selectable.
	if(currentlyChosenTransformationTool->getActiveObject())
	{
		currentlyChosenTransformationTool->tryForSelection(rayOrigin, rayDir, camView);
	}

	// If a transformation tool handle is selected, it should be noted that the selection is already final with this function call,
	// and so there is no need to call finalizeSelection elsewhere.
	
	bool aTransformationToolWasSelected = false;
	if(aTransformationToolWasSelected)
		isSelected = false;
}

/* Called to get the last selected object(s). */
//std::vector<int> &Tool_Selection::getSelectedObjects()
//{
//	return objectsThatHaveBeenSelected;
//}

/* Called to see if the translation tool is (still) active. */
bool Tool_Selection::getIsSelected()
{
	return isSelected;
}

/* Called to send updated parameters to the translation tool, if it is still active. */
void Tool_Selection::update(XMVECTOR &rayOrigin, XMVECTOR &rayDir, Camera &theCamera, D3D11_VIEWPORT &theViewport, POINT &mouseCursorPoint)
{
	// Use the updated mouse cursor position to re-create the selection rectangle.
	
	bool selectOnlyTheClosest = false;
	currentMouseCursorPoint = mouseCursorPoint;
	if(		abs(currentMouseCursorPoint.x - firstMouseCursorPoint.x) < 4
		&&	abs(currentMouseCursorPoint.y - firstMouseCursorPoint.y) < 4	)
	{
		currentMouseCursorPoint.x = firstMouseCursorPoint.x + 4;
		currentMouseCursorPoint.y = firstMouseCursorPoint.y + 4;
		selectOnlyTheClosest = true;
	}

	MyRectangle currentSelectionRectangle;
	currentSelectionRectangle.P1 = XMFLOAT3((float)firstMouseCursorPoint.x, (float)currentMouseCursorPoint.y, 0.0f);
	currentSelectionRectangle.P2 = XMFLOAT3((float)firstMouseCursorPoint.x, (float)firstMouseCursorPoint.y, 0.0f);
	currentSelectionRectangle.P3 = XMFLOAT3((float)currentMouseCursorPoint.x, (float)firstMouseCursorPoint.y, 0.0f);
	currentSelectionRectangle.P4 = XMFLOAT3((float)currentMouseCursorPoint.x, (float)currentMouseCursorPoint.y, 0.0f);

	/*	If many objects are selected, the one that defaults to "the" active one in the group, in/around which the transformation controls appear,
		is the one that was placed last in the scene. */

	// Check against the bounding volumes and bounding geometry of all the scene objects.
	//for(unsigned int i = 0; i < selectableObjects.size(); ++i)
	//{
	//	// Get the bounding volume of the object.
	//	selectableObjects.at(i);

	//	//RectangleSelect(selectableObjects, theViewport, theCamera, currentSelectionRectangle);

	//	//// Check the type and perform the relevant test.
	//	//BoundingSurfaceType BVT;
	//	//switch (BVT)
	//	//{
	//	//	case BVT_TRIANGLE_LIST:
	//	//		break;
	//	//	case BVT_LINE_LIST:
	//	//		break;
	//	//	default:
	//	//		break;
	//	//}
	//}
}

/* Called when the selection tool is unselected, which makes any hitherto made selection final (and undoable). */
void Tool_Selection::finalizeSelection(XMVECTOR &rayOrigin, XMVECTOR &rayDir, Camera &theCamera, D3D11_VIEWPORT &theViewport, POINT &mouseCursorPoint)
{
	currentMouseCursorPoint = mouseCursorPoint;



	// Set all would-be-selected objects as have-been-selected objects.
	for(unsigned int i = 0; i < objectsThatWouldBeSelected.size(); ++i)
	{
		objectsThatHaveBeenSelected.push_back(objectsThatWouldBeSelected.at(i));
	}

	objectsThatWouldBeSelected.clear();

	isSelected = false;
}
	