#pragma once

#include "IData.h"
#include "Math.h"

namespace Data
{
	class Camera : public Type<Camera>
	{
	private:
		Matrix m_mat_projection;
		Matrix m_mat_view;
		Vector3 m_look;
		Vector3 m_right;		
		Vector3 m_up;			
		float m_aspectRatio;
		float m_farPlane;
		float m_fov;
		float m_nearPlane;
		float m_walkingSpeed;

	public:
		Camera()
		{
			m_look = Vector3(0.0f, 0.0f, 1.0f);
			m_right = Vector3(1.0f, 0.0f, 0.0f);
			m_up = Vector3(0.0f, 1.0f, 0.0f);
			m_walkingSpeed = 8.0f;
		}

		void updateViewMatrix(Vector3& position)
		{
			// Keep camera's axes orthogonal to each other and of unit length.
			m_look.Normalize();
			m_up = m_look.Cross(m_right); m_up.Normalize();

			// Look and Up is already orthogonal
			// no need to normalize cross product
			m_right = m_up.Cross(m_look);

			
			float x = -position.Dot(m_right);
			float y = -position.Dot(m_up);
			float z = -position.Dot(m_look);

			// Create view
			m_mat_view(0,0) = m_right.x; 
			m_mat_view(1,0) = m_right.y; 
			m_mat_view(2,0) = m_right.z; 
			m_mat_view(3,0) = x;   

			m_mat_view(0,1) = m_up.x;
			m_mat_view(1,1) = m_up.y;
			m_mat_view(2,1) = m_up.z;
			m_mat_view(3,1) = y;  

			m_mat_view(0,2) = m_look.x; 
			m_mat_view(1,2) = m_look.y; 
			m_mat_view(2,2) = m_look.z; 
			m_mat_view(3,2) = z;   

			m_mat_view(0,3) = 0.0f;
			m_mat_view(1,3) = 0.0f;
			m_mat_view(2,3) = 0.0f;
			m_mat_view(3,3) = 1.0f;
		}

		void setLens(float p_fov_y, float p_aspectRatio, float p_nearPlane, float p_farPlane)
		{
			// Save properties
			m_fov = p_fov_y;
			m_aspectRatio = p_aspectRatio;
			m_nearPlane = p_nearPlane;
			m_farPlane = p_farPlane;

			// Set view
			m_mat_projection = Matrix::CreatePerspectiveFieldOfView(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
		};

		void rotateZ(float p_angle)
		{
			Matrix mat_rot = Matrix::CreateFromAxisAngle(m_right, p_angle);
			m_up = Vector3::TransformNormal(m_up, mat_rot);
			m_look = Vector3::TransformNormal(m_look, mat_rot);
		}

		void rotateY(float p_angle)
		{
			Matrix mat_rot = Matrix::CreateRotationY(p_angle);

			m_right = Vector3::TransformNormal(m_right, mat_rot);
			m_up = Vector3::TransformNormal(m_up, mat_rot);
			m_look = Vector3::TransformNormal(m_look, mat_rot);
		}

		void getPickingRay(Vector2 p_pickedCordinate, Vector2 p_screenSize, Vector3 &p_rayOrigin, Vector3 &p_rayDir)
		{
			// Compute picking ray in view space.
			float vx = (+2.0f*p_pickedCordinate.x/p_screenSize.x  - 1.0f)/m_mat_projection(0,0);
			float vy = (-2.0f*p_pickedCordinate.y/p_screenSize.y + 1.0f)/m_mat_projection(1,1);

			// Ray definition in view space.
			p_rayOrigin = Vector3(0.0f, 0.0f, 0.0f);
			p_rayDir    = Vector3(vx, vy, 1.0f);
		}

		void lookAt(const Vector3& p_cameraPos, const Vector3& p_target, const Vector3& p_up)
		{
			m_look = p_target - p_cameraPos; m_look.Normalize();
			m_right = m_look.Cross(p_up); m_right.Normalize();
			m_up = m_look.Cross(m_right);
		}

		void strafe(Vector3& p_cameraPos, float p_distance)
		{
			//Vector3 s(p_distance);
			//Vectore pos;

			//Vector3 r(d);
			//// mPosition += d*mRight
			//XMVECTOR s = XMVectorReplicate(d);
			//XMVECTOR r = XMLoadFloat3(&mRight);
			//XMVECTOR p = XMLoadFloat3(&mPosition);
			//XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, r, p));
		}

	public:
		Vector3 look()
		{
			return m_look;
		}
		Vector3 up()
		{
			return m_up;
		}
		Matrix viewProjection()
		{
			Matrix viewProjection = m_mat_view*m_mat_projection;
			return viewProjection;
		}
		Matrix view()
		{
			return m_mat_view;
		}
		Matrix projection()
		{
			return m_mat_projection;
		}
	};

}

