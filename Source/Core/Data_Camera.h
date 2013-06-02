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
		float m_scale;

		Vector3 m_position;
		Quaternion m_rotation;

	public:
		Camera()
		{
			m_look = Vector3(0.0f, 0.0f, 1.0f);
			m_right = Vector3(1.0f, 0.0f, 0.0f);
			m_up = Vector3(0.0f, 1.0f, 0.0f);
			m_scale = 1.0f;
		}

		void updateViewMatrix(Vector3& position)
		{
			m_position = position;

			// Keep camera's axes orthogonal to each other and of unit length.
			m_look.Normalize();
			m_up = m_look.Cross(m_right); m_up.Normalize();

			// Look and Up is already orthogonal
			// no need to normalize cross product
			m_right = m_up.Cross(m_look);

			// Create LookAt
			m_mat_view = Matrix::CreateLookAt(position, position + m_look, m_up);

			m_rotation = rotation();
		}

		void setLens(float p_fov_y, float p_aspectRatio, float p_nearPlane, float p_farPlane);


		void rotateX(float p_angle)
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

		void getPickingRay(Vector2 p_pickedCordinate, Vector2 p_screenSize, Vector4& p_rayOrigin, Vector3& p_rayDir)
		{
			// Compute picking ray in view space.
			float vx = (+2.0f*p_pickedCordinate.x/p_screenSize.x  - 1.0f)/m_mat_projection(0,0);
			float vy = (-2.0f*p_pickedCordinate.y/p_screenSize.y + 1.0f)/m_mat_projection(1,1);

			// Ray definition in view space.
			p_rayOrigin = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
			p_rayDir    = Vector3(vx, vy, 1.0f);
		}

		void getPickingRay(Float2& p_pickedCordinate, Float2 p_screenSize, Ray* p_ray)
		{
			// Compute picking ray in view space.
			float vx = (+2.0f*p_pickedCordinate.x/p_screenSize.x  - 1.0f)/m_mat_projection(0,0);
			float vy = (-2.0f*p_pickedCordinate.y/p_screenSize.y + 1.0f)/m_mat_projection(1,1);

			// Ray definition in view space.
			p_ray->position = Vector3(0.0f, 0.0f, 0.0f);
			p_ray->direction = Vector3(vx, vy, 1.0f);
		}

		void lookAt(const Vector3& p_cameraPos, const Vector3& p_target, const Vector3& p_up)
		{
			m_look = p_target - p_cameraPos; m_look.Normalize();
			m_right = m_look.Cross(p_up); m_right.Normalize();
			m_up = m_look.Cross(m_right);
		}

		void setLookVector(const Vector3& p_lookVector, const Vector3& p_up)
		{
			m_look = p_lookVector; m_look.Normalize();
			m_right = m_look.Cross(p_up); m_right.Normalize();
			m_up = m_look.Cross(m_right);
		}

		void strafe(Vector3& p_cameraPos, float p_distance)
		{
			p_cameraPos = p_cameraPos + m_right * p_distance;
		}

		void walk(Vector3& p_cameraPos, float p_distance)
		{
			p_cameraPos = p_cameraPos + m_look * p_distance;
		}

		void ascend(Vector3& p_cameraPos, float p_distance)
		{
			p_cameraPos = p_cameraPos + m_up * p_distance;
		}

		BoundingFrustum toFrustum();

		BoundingFrustum getSubFrustum(FloatRectangle& window, FloatRectangle& sub_window);


	public:
		Vector3 look()
		{
			return m_look;
		}
		Vector3 up()
		{
			return m_up;
		}
		Vector3 getLookVector()
		{
			return m_look;
		}

		Vector3 getRightVector()
		{
			return m_right;
		}
		Matrix viewProjection()
		{
			Matrix m = m_mat_view*m_mat_projection;
			return m;
		}
		Matrix view()
		{
			return m_mat_view;
		}
		Matrix projection()
		{
			return m_mat_projection;
		}

		Quaternion rotation(Vector3& position);
		Quaternion rotation();

		void setScale(float value);
		float scale(){return m_scale;}
	};
}

