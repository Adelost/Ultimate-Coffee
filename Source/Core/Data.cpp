#include "stdafx.h"
#include "Data.h"

#include "xnacollision.h"

#include "DataMapper.h"
#include "Entity.h"

EntityPointer Data::Selected::lastSelected;


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
		Entity* e = map_selected.nextEntity();
		e->removeData<Data::Selected>();
	}

	// All selection was cleared, no entity is selected
	Data::Selected::lastSelected.invalidate();
}

void Data::Selected::select( Entity* e )
{
	e->addData(Data::Selected());
	Data::Selected::lastSelected = e->toPointer();
}

void Data::Selected::unselect( Entity* e )
{
	e->removeData<Data::Selected>();

	// Find new Entity to select if we unselected our last entity
	if(Data::Selected::lastSelected == e)
	{
		findLastSelected();
	}
}

void Data::Selected::findLastSelected()
{
	Data::Selected::lastSelected.invalidate();
	DataMapper<Data::Selected> map_selected;
	if(map_selected.hasNext())
	{
		Data::Selected::lastSelected = map_selected.nextEntity()->toPointer();
	}
}
