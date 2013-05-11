#include "stdafx.h"
#include "Data.h"

#include "xnacollision.h"

#include "DataMapper.h"
#include "Entity.h"

EntityPointer Data::Selected::pivot;


Entity* Data::Bounding::intersect( const Ray& ray )
{
	Entity* out = nullptr;

	DataMapper<Data::Bounding> map_bounding;
	while(map_bounding.hasNext())
	{
		Entity* entity = map_bounding.nextEntity();
		Data::Bounding* d_bounding  = entity->fetchData<Data::Bounding>();
		if(d_bounding->intersect(entity, ray))
			out = entity;
	}

	return out;
}

bool Data::Bounding::intersect( Entity* entity, const Ray& ray )
{
	// Create bounding shape
	Data::Transform* d_transform  = entity->fetchData<Data::Transform>();
	BoundingSphere sphere(d_transform->position, 1.0f);

	// Perform intersection test
	float d;
	bool out = ray.Intersects(sphere, d);

	return out;
}

void Data::Selected::clearSelection()
{
	DataMapper<Data::Selected> map_selected;
	while(map_selected.hasNext())
	{
		Entity* entity = map_selected.nextEntity();
		entity->removeData<Data::Selected>();
	}
}
