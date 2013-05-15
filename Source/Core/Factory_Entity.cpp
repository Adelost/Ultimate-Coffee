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
		static int id = 0;
		e->setName("empty", id);
		id++;
	}

	if(p_type == ENTITY_CUBE)
	{
		static int id = 0;
		e->setName("cube", id);
		id++;

		// Randomize position
		Data::Transform* d_transform = e->addData(Data::Transform());
		d_transform->position.x = Math::randomFloat(-0.0f, 1.0f);
		d_transform->position.y = Math::randomFloat(-0.2f, 0.2f);
		d_transform->position.z = Math::randomFloat(-0.0f, 1.0f);
		static float d = 10.0f;
		d += 0.3f;
		d_transform->position *= d;
		d_transform->rotation = Quaternion::CreateFromYawPitchRoll(Math::randomFloat(0.0f, Math::Pi*2), Math::randomFloat(0.0f, Math::Pi*2), Math::randomFloat(0.0f, Math::Pi*2));
		
		e->addData(Data::Bounding());
		e->addData(Data::Render());
		e->addData(Data::Update());
	}

	if(p_type == ENTITY_CAMERA)
	{
		static int id = 0;
		e->setName("camera", id);
		id++;

		Data::Transform* d_transform = e->addData(Data::Transform());
		d_transform->position = Vector3(0.0f, 0.0f, -15.0f);

		Data::Camera* d_camera = e->addData(Data::Camera());
		d_camera->updateViewMatrix(d_transform->position);
	}

	return e;
}
