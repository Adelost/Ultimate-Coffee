#include "stdafx.h"
#include "Data_Camera.h"

DirectX::SimpleMath::Quaternion Data::Camera::rotation()
{
	// Local to World
	Matrix m = m_mat_view.Invert();

	// Return as Quaternion
	return Quaternion::CreateFromRotationMatrix(m);
}

DirectX::BoundingFrustum Data::Camera::toFrustum()
{
	BoundingFrustum f(m_mat_projection);
	f.Origin = m_position;
	f.Orientation = m_rotation;

	return f;
}

void Data::Camera::setLens( float fov_y, float aspectRatio, float nearPlane, float farPlane )
{
	// Save properties
	m_fov = fov_y;
	m_aspectRatio = aspectRatio;
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;

	// Compensate for scale
	nearPlane = m_nearPlane * m_scale;
	farPlane =  m_farPlane * m_scale;

	DEBUGPRINT("Delta zoom: " << nearPlane - farPlane);
// 
// 	// Don't shrink far plane
// 	if(farPlane < m_farPlane)
// 		farPlane = m_farPlane;

	// Set view
	m_mat_projection = Matrix::CreatePerspectiveFieldOfView(m_fov, m_aspectRatio, nearPlane, farPlane);
}

DirectX::BoundingFrustum Data::Camera::getSubFrustum( FloatRectangle& window, FloatRectangle& sub_window )
{
	// Construct bounding frustum from camera projection
	BoundingFrustum f(m_mat_projection);
	f.Origin = m_position;
	f.Orientation = m_rotation;

	// Translates the rectangle from "0 .. 2x", to "-x .. x"
	// making center 0,0 in the new coordinate system
	Float2 center = window.center();
	window.translate(-center);
	sub_window.translate(-center);

	// Adjust bounding slopes to match sub window
	float left = sub_window.position.x/window.position.x;
 	float right = (sub_window.position.x + sub_window.size.x)/(window.position.x + window.size.x);
 	float top =  sub_window.position.y/window.position.y;
 	float bottom =  (sub_window.position.y + sub_window.size.y)/(window.position.y + window.size.y);
 	f.LeftSlope *= left;
 	f.RightSlope *= right;
 	f.TopSlope *= top;
	f.BottomSlope *= bottom;

	return f;
}

void Data::Camera::setScale( float value )
{
	m_scale = value;

	// Recalculate lens
	setLens(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
}

void Data::Camera::updateViewMatrix( Vector3& position )
{
	m_position = position;

	// Keep camera's axes orthogonal to each other and of unit length.
	m_look.Normalize();
	m_up = m_look.Cross(m_right); m_up.Normalize();
	// Look and Up is already orthogonal
	// no need to normalize cross product
	m_right = m_up.Cross(m_look);

	// Apply offset orientation
	Matrix offset = Matrix::CreateFromQuaternion(m_rotationOffset);
	Vector3 look = Vector3::TransformNormal(m_look, offset);
	Vector3 up = Vector3::TransformNormal(m_up, offset);
	Vector3 right = Vector3::TransformNormal(m_right, offset);

	// Create LookAt
	m_mat_view = Matrix::CreateLookAt(position, position + look, up);

	Matrix oculusOffset = Matrix::CreateFromQuaternion(m_rotationOffset);
	m_rotation = rotation();
}
