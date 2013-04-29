#ifndef HANDLE_TRANSLATIONAXIS_H
#define HANDLE_TRANSLATIONAXIS_H

#include "DirectXMath.h"
using namespace DirectX;

class Handle_TranslationAxis
{
private:
	XMFLOAT4X4 world;
	XMFLOAT3 direction; // Should be X, Y, or Z.

	// The delta between the first and last picked point on the axis plane gives the current translation from the active object's last set position.
	XMFLOAT3 prevPickedPointOnAxisPlane;
	XMFLOAT3 nextPickedPointOnAxisPlane;

	bool isSelected;

public:
	Handle_TranslationAxis(XMVECTOR &direction);
	~Handle_TranslationAxis();
	
	/* Called for initial selection and picking against the axis plane. */
	bool tryForSelection(XMVECTOR &pickingRay); 
	
	/* Called for continued picking against the axis plane. */
	void pickAxisPlane(XMVECTOR &pickingRay);

	/* Called when picking against the axis plane should cease, and the last translation made final. */
	void unselect(XMVECTOR &pickingRay);
	
	/* Called to see if this is the currently selected translation axis, if any. */
	bool getIsSelected();

	/* Called to retrieve the last made translation delta. */
	XMVECTOR getLastTranslationDelta();

	/* Called for the needed transform of the visual and/or bounding components of the handle. */
	void setWorld(XMMATRIX &world);
};

#endif