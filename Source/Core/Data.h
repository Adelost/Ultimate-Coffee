#pragma once

#include "IData.h"
#include "Math.h"
#include "Data_Camera.h"

namespace Data
{
	class Transform : public Type<Transform>
	{
	public:
		Vector3 position;
		Quaternion rotation;
		Vector3 scale;

	public:
		Transform()
		{
			scale = Vector3(1.0f, 1.0f, 1.0f);
		}

		Matrix toWorldMatrix()
		{
			Matrix mat_pos;
			mat_pos = Matrix::CreateTranslation(position);
			Matrix mat_rot;
			mat_rot = Matrix::CreateFromQuaternion(rotation);
			Matrix mat_scale;
			mat_scale = Matrix::CreateScale(scale);

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
}

