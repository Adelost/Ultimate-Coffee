#include "stdafx.h"
#include "Data_Camera.h"

Quaternion Data::Camera::rotation(Vector3& position)
{
	// Local to World
	Matrix m = m_mat_view.Invert();

	// Return as Quaternion
	return Quaternion::CreateFromRotationMatrix(m);
}

DirectX::BoundingFrustum Data::Camera::toFrustum( Vector3& cameraPos, Quaternion& cameraRot )
{
	BoundingFrustum f(m_mat_projection);
	f.Origin = cameraPos;
	f.Orientation = cameraRot;

	return f;
}

