#include "stdafx.h"
#include "Data_Camera.h"

Quaternion Data::Camera::rotation(Vector3& position)
{
	// Local to World
	Matrix m = m_mat_view.Invert();

	// Return as Quaternion
	return Quaternion::CreateFromRotationMatrix(m);
}