#pragma once

#include <vector>
#include "IData.h"
#include "Math.h"
#include "Data_Camera.h"
#include "EntityPointer.h"
#include "Batch.h"
#include "Enums.h"

class Entity;
class Buffer;

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
		static void intersect(const BoundingFrustum& frustum, std::vector<Entity*>* entity_list);
		bool intersect(Entity* entity, const Ray& ray, float* distance);
	};

	/**
	Should contain everything render needs.
	Position should be fetched from Translation.
	*/
	class Render : public Type<Render>
	{
	public:
		class BufferStore
		{
		public:
			Buffer* vertex;
			Buffer* index;

		public:
			BufferStore()
			{
				vertex = nullptr;
				index = nullptr;
			}
		};

		/**
		Helps rendering do some optimization.
		*/
		class Manager
		{
		public:
			std::vector<Batch<EntityPointer>> renderBatch_list;
			std::vector<BufferStore> buffer_list;
		};

		class MeshInfo
		{
		public:
			int id;
			int index;
			Color color;

		public:
			MeshInfo()
			{
				id = -1;
				index = -1;
			}
		};

	public:
		static Manager manager;

	public:
		MeshInfo mesh;
		EntityPointer owner;
		
	public:
		Render(Entity* entity, int meshId);
		static void initClass()
		{
			// Build render batches
			for(int i=0; i<Enum::Mesh_End; i++)
			{
				manager.renderBatch_list.push_back(Batch<EntityPointer>());
				manager.buffer_list.push_back(BufferStore());
			}
		}
		void clean()
		{
			clearMesh();
		}

	public:
		void setMesh(int meshId)
		{
			clearMesh();

			int index = manager.renderBatch_list[meshId].addItem(owner);
			mesh.id = meshId;
			mesh.index = index;
		}
		void clearMesh()
		{
			if(mesh.index != -1)
			{
				manager.renderBatch_list[mesh.id].removeItemAt(mesh.index);
			}
		}
	};


	class Movement_Floating : public Type<Movement_Floating>
	{
	public:
		Vector3 direction;
		Vector3 rotation;
		float speed;

	public:
		Movement_Floating();
	};

	/**
	Should contain point light information.
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

	class Created : public Type<Created>{};
	class Deleted : public Type<Deleted>{};

	/**
	Used to quickly move to an other Entity.
	*/
	class ZoomTo : public Type<ZoomTo>
	{
	public:
		EntityPointer target;
		float delay; // Used for linear interpolation perhaps?
	};
}

