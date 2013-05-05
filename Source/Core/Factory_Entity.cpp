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
		// Randomize position
		Data::Transform* d_transform = e->addData(Data::Transform());
		d_transform->position.x = Math::randomFloat(-3.0f, 3.0f);
		d_transform->position.y = Math::randomFloat(-3.0f, 3.0f);
		d_transform->position.z = Math::randomFloat(-3.0f, 3.0f);
		
		e->addData(Data::Render());
	}

	if(p_type == ENTITY_CAMERA)
	{
		Data::Transform* d_transform = e->addData(Data::Transform());
		d_transform->position = Vector3(0.0f, 0.0f, -15.0f);

		Data::Camera* d_camera = e->addData(Data::Camera());
		d_camera->updateViewMatrix(d_transform->position);
	}

	return e;
}
