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

		Matrix toWorldMatrix();
		Matrix toRotPosMatrix();
		Matrix toPosMatrix();
		Matrix toRotMatrix();
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
		static void select(Entity* e);
		static void unselect(Entity* e);
		static void findLastSelected();
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
		bool intersect(Entity* entity, const Ray& ray, float* distance);
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


	class Update : public Type<Update>
	{
	public:
		Vector3 direction;
		Vector3 rotation;
		float speed;

	public:
		Update();
	};

	/**
	Should containt point light information.
	Position should be fetched from Translation.
	*/
	class PointLight : public Type<PointLight>
	{
	public:
		Vector3 color;
		float range;
	};

	class Sky : public Type<Sky>
	{
	public:
	};
}

