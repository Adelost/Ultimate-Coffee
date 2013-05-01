#include "Handle_TranslationAxis.h"

Handle_TranslationAxis::Handle_TranslationAxis(XMVECTOR &direction)
{
	XMStoreFloat3(&this->direction, direction);
}

Handle_TranslationAxis::~Handle_TranslationAxis()
{
}

/* Called for initial selection and picking against the axis plane. */
bool Handle_TranslationAxis::tryForSelection(XMVECTOR &pickingRay)
{
	bool wasSelected = false;

	// Calculate where the picking ray that selected the handle intersects with the plane the handle lies in.
	// ...

	isSelected = wasSelected;

	return wasSelected;
}
	
/* Called for continued picking against the axis plane, if LMB has yet to be released. */
void Handle_TranslationAxis::pickAxisPlane(XMVECTOR &pickingRay)
{
	prevPickedPointOnAxisPlane = nextPickedPointOnAxisPlane;

	// Calculate where the picking ray that selected the handle intersects with the plane the handle lies in.
	// ...

	XMVECTOR loadedDirection = XMLoadFloat3(&direction);
	XMVECTOR loadedPrevPointOnAxisPlane = XMLoadFloat3(&prevPickedPointOnAxisPlane);
	XMVECTOR intersectionPoint = loadedPrevPointOnAxisPlane + loadedDirection * 0.1f;

	XMStoreFloat3(&nextPickedPointOnAxisPlane, intersectionPoint);
}

/* Called when picking against the axis plane should cease, if the LMB has been released. */
void Handle_TranslationAxis::unselect(XMVECTOR &pickingRay)
{
}
	
/* Called to see if this is the currently selected translation axis, if any. */
bool Handle_TranslationAxis::getIsSelected()
{
	return isSelected;
}

/* Called to retrieve the last made translation delta. */
XMVECTOR Handle_TranslationAxis::getLastTranslationDelta()
{
	XMVECTOR loadedNextPickedPointOnAxisPlane; XMLoadFloat3(&nextPickedPointOnAxisPlane);
	XMVECTOR loadedPrevPickedPointOnAxisPlane; XMLoadFloat3(&prevPickedPointOnAxisPlane);
	XMVECTOR diffVector = loadedNextPickedPointOnAxisPlane - loadedPrevPickedPointOnAxisPlane;
	
	XMVECTOR loadedDirection = XMLoadFloat3(&direction);
	XMFLOAT3 tempVector(	loadedDirection.m128_f32[0] * diffVector.m128_f32[0],
							loadedDirection.m128_f32[1] * diffVector.m128_f32[1],
							loadedDirection.m128_f32[2] * diffVector.m128_f32[2]	);
	
	return XMLoadFloat3(&tempVector);
}

/* Called for the needed transform of the visual and/or bounding components of the handle. */
void Handle_TranslationAxis::setWorld(XMMATRIX &world)
{
	XMStoreFloat4x4(&this->world, world);
}