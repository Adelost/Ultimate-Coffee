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

DirectX::BoundingFrustum Data::Camera::getSubFrustum( FloatRectangle& window, FloatRectangle& sub_window )
{
	float sub_w_aspectRatio = sub_window.size.yRatio();
	float sub_w_sizeYRatio = sub_window.sizeY() / window.sizeY();
	float sub_w_fov = m_fov * sub_w_sizeYRatio;

	Matrix projection = Matrix::CreatePerspectiveFieldOfView(sub_w_fov, sub_w_aspectRatio, m_nearPlane, m_farPlane);
	BoundingFrustum out(projection);
	out.Origin = m_position; // Same as parent frustum

	// Compute sub window rotation by measuring the angle
	// of two picked rays on the parent window relative to 
	// the center of each window
	Float2 w_center = window.center();
	Float2 sub_w_center = sub_window.center();
	//sub_w_center.x *= -1.0f;
	//sub_w_center.y *= -1.0f;

	Ray w_ray;
	getPickingRay(w_center, window.size, &w_ray);
	Ray sub_w_ray;
	getPickingRay(sub_w_center, window.size, &sub_w_ray);

	Vector3 w_dir = w_ray.direction;
	Vector3 sub_w_dir = sub_w_ray.direction;

	// Create rotation matrix from the two vectors
	Vector3 asd = sub_w_dir.Cross(m_right);
	Matrix mat_subRot = Matrix::CreateLookAt(Vector3(0.0f, 0.0f, 0.0f), sub_w_dir, asd);
	mat_subRot = mat_subRot.Invert();
	Quaternion qua_subRot = Quaternion::CreateFromRotationMatrix(mat_subRot);

	out.Orientation = m_rotation * qua_subRot;

	return out;
}

