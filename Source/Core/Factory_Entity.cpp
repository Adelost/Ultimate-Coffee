#include "stdafx.h"
#include "Factory_Entity.h"

#include "World.h"
#include "Manager_Entity.h"
#include "Data.h"
#include "Data_Camera.h"
#include "Command_CreateEntity.h"
#include "Data_Camera.h"
#include "Events.h"

Factory_Entity::Factory_Entity()
{
	m_manager_entity = WORLD()->manager_entity();
}

Entity* Factory_Entity::createEntity(Enum::EntityType type, bool addToHistory)
{
	Entity* e = m_manager_entity->create();
	e->setType(type);

	if(type == Enum::Entity_Empty)
	{
		e->setName("empty", e->uniqueId());
	}

	if(type == Enum::Entity_Sky)
	{
		e->setName("skybox", e->uniqueId());

		e->addData(Data::Sky());
		e->addData(Data::Transform());
	}


	if(type == Enum::Entity_Cube)
	{
		e->setName("cube", e->uniqueId());

		// Randomize position
		Data::Transform* d_transform = e->addData(Data::Transform());
		d_transform->position.x = Math::randomFloat(-0.0f, 1.0f);
		d_transform->position.y = Math::randomFloat(-0.2f, 0.2f);
		d_transform->position.z = Math::randomFloat(-0.0f, 1.0f);
		float d = 10.0f + 0.3f * e->id();
		d_transform->position *= d;
		d_transform->rotation = Quaternion::CreateFromYawPitchRoll(Math::randomFloat(0.0f, Math::Pi*2), Math::randomFloat(0.0f, Math::Pi*2), Math::randomFloat(0.0f, Math::Pi*2));
		
		e->addData(Data::Bounding());
		e->addData(Data::Render());
	}

	if(type == Enum::Entity_Asteroid)
	{
		e->setName("asteroid", e->uniqueId());

		// Randomize position
		Data::Transform* d_transform = e->addData(Data::Transform());
		d_transform->position.x = Math::randomFloat(-0.0f, 1.0f);
		d_transform->position.y = Math::randomFloat(-0.2f, 0.2f);
		d_transform->position.z = Math::randomFloat(-0.0f, 1.0f);
		float d = 10.0f + 0.3f * e->id();
		d_transform->position *= d;
		d_transform->rotation = Quaternion::CreateFromYawPitchRoll(Math::randomFloat(0.0f, Math::Pi*2), Math::randomFloat(0.0f, Math::Pi*2), Math::randomFloat(0.0f, Math::Pi*2));

		e->addData(Data::Bounding());
		e->addData(Data::Render());
		e->addData(Data::Update());
	}

	if(type == Enum::Entity_Camera)
	{
		e->setName("camera", e->uniqueId());

		Data::Transform* d_transform = e->addData(Data::Transform());
		d_transform->position = Vector3(0.0f, 0.0f, -15.0f);

		Data::Camera* d_camera = e->addData(Data::Camera());
		d_camera->updateViewMatrix(d_transform->position);
	}

	if(type == Enum::Entity_Pointlight)
	{
		static int id = 0;
		e->setName("point_light", e->uniqueId());
		id++;

		Data::Transform* transform = e->addData(Data::Transform());
		transform->position = Vector3(30.0f * id, 0.0f, 30.0f * id);
		transform->scale = Vector3(0.1f, 0.1f, 0.1f);
		
		Data::PointLight* pointLight = e->addData(Data::PointLight());
		switch(id)
		{
		case 1:
			pointLight->color = Vector3(1.0f, 0.0f, 0.0f);
			break;
		case 2:
			pointLight->color = Vector3(0.0f, 1.0f, 0.0f);
			break;
		case 3:
			pointLight->color = Vector3(0.0f, 0.0f, 1.0f);
			break;
		case 4:
			pointLight->color = Vector3(1.0f, 1.0f, 0.0f);
			break;
		}
		pointLight->range = 50.0f;

	}

	// Add to history
	if(addToHistory)
		SEND_EVENT(&Event_StoreCommandInCommandHistory(new Command_CreateEntity(e), false));

	return e;
}