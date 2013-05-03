#pragma once

#include "IData.h"
#include "Math.h"

namespace Data
{
	class Transform : public Type<Transform>
	{
	public:
		Vector3 position;
		Quaternion rotation;
		Vector3 scale;

	public:
		Matrix toWorldMatrix()
		{
			Matrix mat_pos;
			mat_pos.CreateTranslation(position);
			Matrix mat_rot;
			mat_rot.CreateFromQuaternion(rotation);
			Matrix mat_scale;
			mat_scale.CreateScale(scale);

			Matrix m = mat_scale*mat_rot*mat_pos;
			return m;
		}
	};

	class Bounding : public Type<Bounding>
	{
	public:
		Vector3 position;
		Vector3 size;

	public:
		Matrix toMatrix()
		{
			// TODO
			Matrix m;

			return m;
		}
	};

	
	/**
	Should contain everything render needs.
	Position should be fetched from Translation.
	*/
	class Render : public Type<Render>
	{
	public:
		int meshId;
	};


	class Camera : public Type<Camera>
	{
	public:
		Matrix mat_view;
		Matrix mat_projection;
		float fieldOfView;
		float fieldOfViewModifier;
		float aspectRatio;
		float zNear;
		float zFar;

		Vector3 up;			
		Vector3 right;		
		Vector3 look;
	};
}

