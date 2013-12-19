#pragma once

#include <Core/Math.h>

namespace OVR {class Matrix4f;}

class OculusManager
{
public:
	OculusManager();
	~OculusManager();
	Quaternion GetOrientation();

public:
	void update();
	void updateVars(Enum::Direction direction);
	void getLeftEye();
	float getDistortionScale();
	Matrix getProjectionLeft();
	Matrix matrixRightHandedToLeft(OVR::Matrix4f* matrix);

	int WindowWidth;
	int WindowHeight;

	// Oculus param
	Vector2 LensCenter;
	Vector2 ScreenCenter;
	Vector2 Scale;
	Vector2 ScaleIn;
	Vector4 HmdWarpParam;

	//
	Matrix ViewAdjust;
	Matrix Projection;
	float DistortionScale;
	float YFOVRadians;
	float Aspect;
};