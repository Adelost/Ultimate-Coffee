#ifndef HANDLE_ROTATIONSPHERE_H
#define HANDLE_ROTATIONSPHERE_H

#include "IHandle.h"
#include "Object_Basic.h"
#include "Camera.h"

#include <xnacollision.h>

using XNA::Sphere;

class Handle_RotationSphere : public IHandle
{
private:
	XMFLOAT4X4 world;

	XMFLOAT3 firstPickedPointOnSphere;
	XMFLOAT3 currentlyPickedPointOnSphere;

	Sphere sphere;

	HWND windowHandle;

	bool isSelected;

	XMFLOAT4 lastQuaternionRotationMade;
	XMFLOAT4 quaternionRotationMadeSoFar;

public:
	Handle_RotationSphere(XMVECTOR center, float radius, HWND windowHandle);
	~Handle_RotationSphere();

	/* Called for initial selection and picking against the axis plane. */
	virtual bool tryForSelection(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, float &distanceToIntersectionPoint);

	/* Called for continued picking against the sphere. */
	void pickSphere(XMVECTOR &rayOrigin, XMVECTOR &rayDir, Camera &theCamera, D3D11_VIEWPORT &theViewport, POINT &mouseCursorPoint);

	/* Called when picking against the plane should cease, and the last translation made final. */
	void unselect(XMVECTOR &pickingRay);

	/* Called to see if this is the currently selected rotation handle, if any. */
	bool getIsSelected();

	/* Called to calculate the last made rotation quaternion. */
	void calcLastRotationQuaternion();

	/* Called to retrieve the total rotation quaternion. */
	XMVECTOR getTotalRotationQuaternion();

	/* Called to retrieve the total rotation quaternion. */
	void resetRotationQuaternion();

	/* Called for the needed transform of the visual and/or bounding components of the handle. */
	void setWorld(XMMATRIX &world);

	/* Called to calculate the required translation axis to get rotate a sphere so that point A, on the sphere,
	   would be moved to where point B, on the sphere is, if rotation occured with the angle between the two points on the sphere. */
	XMVECTOR calcRotationAxisFromTwoPointsOnSphere(Sphere &sphere, XMVECTOR &pointA, XMVECTOR &pointB);

	/* Called to calculate the angle between two points on a sphere, so as to be able to rotate A, around the relevant axis, so it ends up where B is. */
	float calcAngleBetweenTwoPointsOnSphere(Sphere &sphere, XMVECTOR &pointA, XMVECTOR &pointB);
};

#endif