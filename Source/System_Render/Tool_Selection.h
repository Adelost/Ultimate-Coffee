#ifndef TOOL_SELECTION_H
#define TOOL_SELECTION_H

#include <vector>

#include "ITool_Transformation.h"
#include "xnacollision.h"

class Tool_Selection
{
private:
	float selectionDepth; // How far from the selection rectangle origin we are to include objects.

	bool isSelecting;

	POINT firstMouseCursorPoint;
	POINT currentMouseCursorPoint;

	std::vector<int> objectsThatHaveBeenSelected;
	std::vector<int> objectsThatWouldBeSelected;

	//BoundingFrustum UnprojectRectangle(Rectangle source, Viewport viewport, Matrix projection, Matrix view)
	//{
	//	http://forums.create.msdn.com/forums/p/6690/35401.aspx , by "The Friggm"
	//	// Many many thanks to him...

	//	// Point in screen space of the center of the region selected
	//	Vector2 regionCenterScreen = new Vector2(source.Center.X, source.Center.Y);

	//	// Generate the projection matrix for the screen region
	//	Matrix regionProjMatrix = projection;

	//	// Calculate the region dimensions in the projection matrix. M11 is inverse of width, M22 is inverse of height.
	//	regionProjMatrix.M11 /= ((float)source.Width / (float)viewport.Width);
	//	regionProjMatrix.M22 /= ((float)source.Height / (float)viewport.Height);

	//	// Calculate the region center in the projection matrix. M31 is horizonatal center.
	//	regionProjMatrix.M31 = (regionCenterScreen.X - (viewport.Width / 2f)) / ((float)source.Width / 2f);

	//	// M32 is vertical center. Notice that the screen has low Y on top, projection has low Y on bottom.
	//	regionProjMatrix.M32 = -(regionCenterScreen.Y - (viewport.Height / 2f)) / ((float)source.Height / 2f);

	//	return new BoundingFrustum(view * regionProjMatrix);
	//}

	//void RectangleSelect(D3D11_VIEWPORT &theViewport, Camera &theCamera, MyRectangle selectionRect)
	//{
	//	// Create a new list to return it

	//	for(unsigned int i = 0; i < possibleSelectees.size(); ++i)
	//	{
	//		// Getting the 2D position of the object
	//		XMVECTOR objectPos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	//		XMVECTOR screenPosForObject = XMVector3Project(	objectPos,
	//														theViewport.TopLeftX,	theViewport.TopLeftY,
	//														theViewport.Width,		theViewport.Height,
	//														theViewport.MinDepth,	theViewport.MaxDepth,
	//														theCamera.Proj(), theCamera.View(), XMMatrixIdentity()	);

	//		//// screenPos is window relative, we change it to be viewport relative
	//		//screenPos.X -= viewport.X;
	//		//screenPos.Y -= viewport.Y;

	//		if (selectionRect.Contains((int)screenPos.X, (int)screenPos.Y))
	//		{
	//			// Add object to selected objects list
	//			selectedObjects.push_back(possibleSelectees.at(i));
	//		}
	//	}

	//	return selectedObjects;
	//}

public:
	Tool_Selection();
	~Tool_Selection();

	/* Called for an instance of picking, possibly resulting in the tool being selected. */
	void beginSelection(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, XMMATRIX &camProj, D3D11_VIEWPORT &theViewport, POINT &mouseCursorPoint, ITool_Transformation *currentlyChosenTransformationTool);

	/* Called to send updated parameters to the translation tool, if it is still in the process of making a selection. */
	void update(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, D3D11_VIEWPORT &theViewport, POINT &mouseCursorPoint);

	/* Called when the selection tool action is "unselected", which makes any hitherto made selection final and undo/redoable. */
	void finalizeSelection(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, D3D11_VIEWPORT &theViewport, POINT &mouseCursorPoint);

	MyRectangle getSelectionRectangle();

	XNA::Frustum getSelectionFrustum();

	bool getIsSelecting();

	void setIsSelecting(bool isSelecting);

	/* Called to get the last selected object(s). */
	//std::vector<IObject*> &getSelectedObjects();
};

#endif