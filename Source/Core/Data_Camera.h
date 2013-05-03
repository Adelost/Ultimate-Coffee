#pragma once

#include "IData.h"
#include "Math.h"

namespace Data
{
	class Camera : public Type<Camera>
	{
	public:
		Matrix mat_projection;
		Matrix mat_view;
		Vector3 look;
		Vector3 right;		
		Vector3 up;			
		float aspectRatio;
		float farPlane;
		float fov;
		float nearPlane;
		float walkingSpeed;

	public:
		Camera()
		{
			look = Vector3(0.0f, 0.0f, 1.0f);
			right = Vector3(1.0f, 0.0f, 0.0f);
			up = Vector3(0.0f, 1.0f, 0.0f);
			walkingSpeed = 8.0f;
		}

		void setView(float p_fov_y, float p_aspectRatio, float p_nearPlane, float p_farPlane)
		{
			// Save properties
			fov = p_fov_y;
			aspectRatio = p_aspectRatio;
			nearPlane = p_nearPlane;
			farPlane = p_farPlane;

			// Set view
			mat_projection.CreatePerspectiveFieldOfView(fov, aspectRatio, nearPlane, farPlane);
		};

		void rotateZ(float p_angle)
		{
			Matrix mat_rot = Matrix::CreateFromAxisAngle(right, p_angle);
			Vector3::TransformNormal(up, mat_rot);
			Vector3::TransformNormal(look, mat_rot);
		}

		void rotateY(float p_angle)
		{
			Matrix mat_rot = Matrix::CreateRotationY(p_angle);

			Vector3::TransformNormal(right, mat_rot);
			Vector3::TransformNormal(up, mat_rot);
			Vector3::TransformNormal(look, mat_rot);
		}

		void getPickingRay(Vector2 p_pickedCordinate, Vector2 p_screenSize, Vector3 &p_rayOrigin, Vector3 &p_rayDir)
		{
			// Compute picking ray in view space.
			float vx = (+2.0f*p_pickedCordinate.x/p_screenSize.x  - 1.0f)/mat_projection._11;
			float vy = (-2.0f*p_pickedCordinate.y/p_screenSize.y + 1.0f)/mat_projection._22;

			// Ray definition in view space.
			p_rayOrigin = Vector3(0.0f, 0.0f, 0.0f);
			p_rayDir    = Vector3(vx, vy, 1.0f);
		}

		void lookAt(const Vector3& p_cameraPos, const Vector3& p_target, const Vector3& p_up)
		{
			Vector3 look = p_target - p_cameraPos; look.Normalize();
			Vector3 right = look.Cross(p_up); right.Normalize();
			Vector3 up = look.Cross(right);
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
	};

}

