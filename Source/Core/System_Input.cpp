#include "stdafx.h"
#include "System_Input.h"

#include "Command_CreateEntity.h"
#include "Factory_Entity.h"
#include "World.h"
#include "Events.h"
#include "Data.h"
#include "World.h"
#include "DataMapper.h"

void System::Input::update()
{
	Entity* entity_camera = CAMERA_ENTITY().asEntity();
	
	Data::Transform* d_transform = entity_camera->fetchData<Data::Transform>();
	Data::Camera* d_camera = entity_camera->fetchData<Data::Camera>();

	float speed = 15.0f * d_camera->scale();
	if(SETTINGS()->button.key_shift)
		speed *= 4.0f;
	float delta = SETTINGS()->trueDeltaTime * speed;
	float strafe = 0.0f;
	float walk = 0.0f;

	if(SETTINGS()->button.key_up)
	{
		walk += delta;
	}
	if(SETTINGS()->button.key_down)
	{
		walk -= delta;
	}
	if(SETTINGS()->button.key_left)
	{
		strafe -= delta;
	}
	if(SETTINGS()->button.key_right)
	{
		strafe += delta;
	}

	// Walk camera
	if(strafe!=0.0f)
		d_camera->strafe(d_transform->position, strafe);
	if(walk!=0.0f)
		d_camera->walk(d_transform->position, walk);
	// Update rotation
	d_transform->rotation = d_camera->rotation();
	d_camera->updateViewMatrix(d_transform->position);

	// Update camera
	d_transform->rotation = d_camera->rotation();
	d_camera->updateViewMatrix(d_transform->position);


	// Do some random stuff
	DataMapper<Data::Movement_Floating> map_update;
	Data::Transform* d_transform_camera = d_transform;
	while(map_update.hasNext())
	{
		Entity* e = map_update.nextEntity();
		if(!e->fetchData<Data::Selected>())
		{
			Data::Transform* d_transform = e->fetchData<Data::Transform>();
			Data::Movement_Floating* d_update = e->fetchData<Data::Movement_Floating>();

			// Hunt camera by applying force
			if(Data::Movement_Floating::targetCamera)
			{
				Vector3 direction = d_transform_camera->position - d_transform->position;
				direction.Normalize();
				d_update->force = direction * 100.0f;
			}

			// Update position
			Vector3 acceleration = d_update->force/ d_update->mass;
			d_update->force = Vector3(0.0f); // Reset force
			d_update->velocity = d_update->velocity + acceleration * SETTINGS()->deltaTime;
			d_transform->position = d_transform->position + d_update->velocity * SETTINGS()->deltaTime;
			

			//d_transform->position = d_transform->position + d_update->direction * d_update->speed * SETTINGS()->deltaTime * 100.0f;

			// Apply rotation
			Vector3 v = d_update->velocity * SETTINGS()->deltaTime;
			Matrix m1 = Matrix::CreateFromQuaternion(d_transform->rotation);
			Matrix m2 = Matrix::CreateFromYawPitchRoll(v.x, v.y, v.z);
			m1 = m1 * m2;
			d_transform->rotation = Quaternion::CreateFromRotationMatrix(m1);
		}
	}


	// Create loots of stuff
	static std::vector<Command*> command_list;
	if(SETTINGS()->selectedTool() == Enum::Tool_Geometry && SETTINGS()->button.mouse_left && SETTINGS()->button.key_ctrl)
	{
		static float cooldown = 0.0f;
		if(cooldown > 0.0f)
			cooldown -= SETTINGS()->trueDeltaTime;
		else
		{
			// Compute picking ray to place Entities onto
			Float2 windowSize((float)SETTINGS()->windowSize.x, (float)SETTINGS()->windowSize.y);
			Ray r;

			Entity* cam = CAMERA_ENTITY().asEntity();
			Data::Transform* d_transform = cam->fetchData<Data::Transform>();
			Data::Camera* d_camera = cam->fetchData<Data::Camera>();
			Int2 mousePos =  SETTINGS()->lastMousePosition;
			d_camera->getPickingRay(Float2((float)mousePos.x, (float)mousePos.y), windowSize, &r);

			// Translate ray to world space
			Matrix mat_world = d_transform->toRotPosMatrix();
			r.position = Vector3::Transform(r.position, mat_world);
			r.direction = Vector3::TransformNormal(r.direction, mat_world);

			// Calculate entity position
			{
				Entity* entity = WORLD()->factory_entity()->createEntity(Enum::Entity_Mesh);
				Data::Transform* d_transform = entity->fetchData<Data::Transform>();
				Vector3 pos = r.position + r.direction * 15.0f;
				d_transform->position = pos;

				// Add to history
				command_list.push_back(new Command_CreateEntity(entity));
			}

			// Add cooldown
			cooldown = 0.03f;
		}
	}
	// If user is not creating more "Random Stuff (tm)"
	// we add the stuff to history
	else
	{
		if(command_list.size()>0)
		{
			SEND_EVENT(&Event_AddToCommandHistory(&command_list))
			command_list.clear();
		}
	}
}
