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
			// TODO
			Matrix m;
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

