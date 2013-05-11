#include "stdafx.h"
#include "Data_Camera.h"

Quaternion Data::Camera::rotation(Vector3& position)
{
	Matrix m = Matrix::CreateLookAt(Vector3(0,0,0), m_look, m_up);
	m = m.Invert();
	return Quaternion::CreateFromRotationMatrix(m);
}