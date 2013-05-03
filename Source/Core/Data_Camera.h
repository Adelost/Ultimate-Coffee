#pragma once

#include "IData.h"
#include "Math.h"

namespace Data
{
	class Camera : public Type<Camera>
	{
	public:
		Matrix mat_view;
		Matrix mat_projection;
		float fov;
		float aspectRatio;
		float nearPlane;
		float farPlane;

		Vector3 up;			
		Vector3 right;		
		Vector3 look;

		float walkingSpeed;

	public:
		Camera()
		{
			right = Vector3(1.0f, 0.0f, 0.0f);
			up = Vector3(0.0f, 1.0f, 0.0f);
			look = Vector3(0.0f, 0.0f, 1.0f);
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
	};

}

