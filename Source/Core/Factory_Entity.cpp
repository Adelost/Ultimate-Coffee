#include "stdafx.h"
#include "Factory_Entity.h"

#include "World.h"
#include "Manager_Entity.h"
#include "Data.h"
#include "Data_Camera.h"

Factory_Entity::Factory_Entity()
{
	m_manager_entity = WORLD()->manager_entity();
}

Entity* Factory_Entity::createEntity( EntityType p_type )
{
	Entity* e = m_manager_entity->create();

	if(p_type == ENTITY_EMPTY)
	{
	}

	if(p_type == ENTITY_CUBE)
	{
		e->addData(Data::Transform());
		e->addData(Data::Render());
	}

	if(p_type == ENTITY_CAMERA)
	{
		e->addData(Data::Transform());
		e->addData(Data::Camera());

		Data::Transform* d_transform;
		d_transform = e->fetchData<Data::Transform>();
		d_transform->position = Vector3(10,5,-15);
		e->fetchData<Data::Camera>()->updateViewMatrix(d_transform->position);
	}

	return e;
}
