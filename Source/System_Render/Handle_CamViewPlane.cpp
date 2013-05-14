#include "stdafx.h"
#include "Core/Math.h"
#include "Handle_CamViewPlane.h"

Handle_CamViewPlane::Handle_CamViewPlane()
{
	mouseXYDeltasMadeSoFar.x = 0;
	mouseXYDeltasMadeSoFar.y = 0;

	mouseXDeltasMadeSoFar = 0;
	mouseYDeltasMadeSoFar = 0;

	shouldFlipCursorAroundSceneView = true;
}

Handle_CamViewPlane::~Handle_CamViewPlane()
{
}

void Handle_CamViewPlane::setShouldFlipCursorAroundSceneView(bool shouldFlipCursor)
{
	shouldFlipCursorAroundSceneView = shouldFlipCursor;
}

void Handle_CamViewPlane::setFirstPointOnViewPlane(POINT mouseCursorPoint)
{
	firstPickedMousePoint = mouseCursorPoint;

	//mouseXYDeltasMadeSoFar.x = 0;
	//mouseXYDeltasMadeSoFar.y = 0;

	//mouseXDeltasMadeSoFar = 0;
	//mouseYDeltasMadeSoFar = 0;

	isSelected = true;
}

void Handle_CamViewPlane::update(POINT currentMouseCursorPoint)
{
	int xDelta = currentMouseCursorPoint.x - firstPickedMousePoint.x;
	int yDelta = currentMouseCursorPoint.y - firstPickedMousePoint.y;

	// If flipping around the screen.

	if(shouldFlipCursorAroundSceneView)
	{
		POINT flippedMouseCursorPoint;
		flippedMouseCursorPoint.x = 0;
		flippedMouseCursorPoint.y = 0;
		if(currentMouseCursorPoint.x <= 5)
		{
			mouseXDeltasMadeSoFar += xDelta;
			mouseYDeltasMadeSoFar += yDelta;

			mouseXYDeltasMadeSoFar.x += xDelta;
			mouseXYDeltasMadeSoFar.y += yDelta;

			flippedMouseCursorPoint.x = SETTINGS()->windowSize.x - 6;
			flippedMouseCursorPoint.y = currentMouseCursorPoint.y;

			Int2 newQtCursorPoint;
			newQtCursorPoint.x = flippedMouseCursorPoint.x;
			newQtCursorPoint.y = flippedMouseCursorPoint.y;
			SEND_EVENT(&Event_SetCursorPosition(newQtCursorPoint));

			firstPickedMousePoint.x = flippedMouseCursorPoint.x;
			firstPickedMousePoint.y = flippedMouseCursorPoint.y;

			currentlyPickedMousePoint.x = firstPickedMousePoint.x;
			currentlyPickedMousePoint.y = firstPickedMousePoint.y;
		}
		else if(currentMouseCursorPoint.x >= SETTINGS()->windowSize.x - 5)
		{
			mouseXDeltasMadeSoFar += xDelta;
			mouseYDeltasMadeSoFar += yDelta;

			mouseXYDeltasMadeSoFar.x += xDelta;
			mouseXYDeltasMadeSoFar.y += yDelta;

			flippedMouseCursorPoint.x = 6;
			flippedMouseCursorPoint.y = currentMouseCursorPoint.y;

			Int2 newQtCursorPoint;
			newQtCursorPoint.x = flippedMouseCursorPoint.x;
			newQtCursorPoint.y = flippedMouseCursorPoint.y;
			SEND_EVENT(&Event_SetCursorPosition(newQtCursorPoint));

			firstPickedMousePoint.x = flippedMouseCursorPoint.x;
			firstPickedMousePoint.y = flippedMouseCursorPoint.y;

			currentlyPickedMousePoint.x = firstPickedMousePoint.x;
			currentlyPickedMousePoint.y = firstPickedMousePoint.y;
		}
		else if(currentMouseCursorPoint.y <= 5)
		{
			mouseXDeltasMadeSoFar += xDelta;
			mouseYDeltasMadeSoFar += yDelta;

			mouseXYDeltasMadeSoFar.x += xDelta;
			mouseXYDeltasMadeSoFar.y += yDelta;

			flippedMouseCursorPoint.x = currentMouseCursorPoint.x;
			flippedMouseCursorPoint.y = SETTINGS()->windowSize.y - 6;

			Int2 newQtCursorPoint;
			newQtCursorPoint.x = flippedMouseCursorPoint.x;
			newQtCursorPoint.y = flippedMouseCursorPoint.y;
			SEND_EVENT(&Event_SetCursorPosition(newQtCursorPoint));

			firstPickedMousePoint.x = flippedMouseCursorPoint.x;
			firstPickedMousePoint.y = flippedMouseCursorPoint.y;

			currentlyPickedMousePoint.x = firstPickedMousePoint.x;
			currentlyPickedMousePoint.y = firstPickedMousePoint.y;
		}
		else if(currentMouseCursorPoint.y >= SETTINGS()->windowSize.y - 5)
		{
			mouseXDeltasMadeSoFar += xDelta;
			mouseYDeltasMadeSoFar += yDelta;

			mouseXYDeltasMadeSoFar.x += xDelta;
			mouseXYDeltasMadeSoFar.y += yDelta;

			flippedMouseCursorPoint.x = currentMouseCursorPoint.x;
			flippedMouseCursorPoint.y = 6;

			Int2 newQtCursorPoint;
			newQtCursorPoint.x = flippedMouseCursorPoint.x;
			newQtCursorPoint.y = flippedMouseCursorPoint.y;
			SEND_EVENT(&Event_SetCursorPosition(newQtCursorPoint));

			firstPickedMousePoint.x = flippedMouseCursorPoint.x;
			firstPickedMousePoint.y = flippedMouseCursorPoint.y;

			currentlyPickedMousePoint.x = firstPickedMousePoint.x;
			currentlyPickedMousePoint.y = firstPickedMousePoint.y;
		}
		else
		{
			currentlyPickedMousePointX = currentMouseCursorPoint.x;
			currentlyPickedMousePointY = currentMouseCursorPoint.y;

			currentlyPickedMousePoint.x = currentMouseCursorPoint.x;
			currentlyPickedMousePoint.y = currentMouseCursorPoint.y;
		}
	}
	else
	{
		currentlyPickedMousePointX = currentMouseCursorPoint.x;
		currentlyPickedMousePointY = currentMouseCursorPoint.y;

		currentlyPickedMousePoint.x = currentMouseCursorPoint.x;
		currentlyPickedMousePoint.y = currentMouseCursorPoint.y;
	}
}

POINT Handle_CamViewPlane::getTotalMouseCursorXYDeltas()
{
	POINT totalMouseXYDeltas;

	//LONG test = firstPickedMousePoint.x;

	int X = (int)(firstPickedMousePoint.x - currentlyPickedMousePointX);
	int Y = (int)(firstPickedMousePoint.y - currentlyPickedMousePointY);

	int totX = mouseXDeltasMadeSoFar + X;
	int totY = mouseXDeltasMadeSoFar + Y;

	if(mouseXDeltasMadeSoFar > 0)
	{
		int test = 4;
	}

	//mouseXDeltasMadeSoFar = 0;

	totalMouseXYDeltas.x = mouseXDeltasMadeSoFar + X;
	totalMouseXYDeltas.y = mouseYDeltasMadeSoFar + Y;

	return totalMouseXYDeltas;
}

bool Handle_CamViewPlane::getIsSelected()
{
	return isSelected;
}

void Handle_CamViewPlane::unselect()
{
	mouseXYDeltasMadeSoFar.x = 0;
	mouseXYDeltasMadeSoFar.y = 0;

	mouseXDeltasMadeSoFar = 0;
	mouseYDeltasMadeSoFar = 0;

	isSelected = false;
}
