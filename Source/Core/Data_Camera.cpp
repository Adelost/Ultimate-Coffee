#include "stdafx.h"
#include "Data_Camera.h"

DirectX::SimpleMath::Quaternion Data::Camera::rotation()
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

void Data::Camera::setLens( float fov_y, float aspectRatio, float nearPlane, float farPlane )
{
	// Save properties
	m_fov = fov_y;
	m_aspectRatio = aspectRatio;
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;

	// Set view
	m_mat_projection = Matrix::CreatePerspectiveFieldOfView(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);

	


	


}

