#pragma once

#include "IData.h"
#include "Math.h"
#include "Data_Camera.h"
#include "EntityPointer.h"

class Entity;

namespace Data
{
	/**
	Represents spatial information
	*/
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
			Matrix mat_pos = Matrix::CreateTranslation(position);
			Matrix mat_rot = Matrix::CreateFromQuaternion(rotation);
			Matrix mat_scale = Matrix::CreateScale(scale);

			Matrix m = mat_scale*mat_rot*mat_pos;
			return m;
		}
	};

	/**
	Used to mark Entities as being selected.
	Could contain additional meta information if needed.
	*/
	class Selected : public Type<Selected>
	{
	public:
		/**
		The Entity which is used as a pivot point e.g. 
		when rotating.
		*/
		static EntityPointer lastSelected;
		static void clearSelection();
	};

	/**
	Should contain some bounding shape information to be
	used in intersection/collision tests.
	*/
	class Bounding : public Type<Bounding>
	{
	public:
		Vector3 position;
		Vector3 size;
		
	public:
		/**
		Returns the Entity intersected with. NULL if
		no intersection.
		*/
		static Entity* intersect(const Ray& ray);
		bool intersect(Entity* entity, const Ray& ray);
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

