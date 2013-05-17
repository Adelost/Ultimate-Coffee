#include "stdafx.h"
#include "System_Input.h"

#include "Data.h"
#include "World.h"
#include "DataMapper.h"

void System::Input::update()
{
	Entity* entity_camera = CAMERA_ENTITY().asEntity();
	
	Data::Transform* d_transform = entity_camera->fetchData<Data::Transform>();
	Data::Camera* d_camera = entity_camera->fetchData<Data::Camera>();

	float speed = 15.0f;
	if(SETTINGS()->button.key_shift)
		speed *= 5.0f;
	float delta = SETTINGS()->deltaTime * speed;
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
	d_transform->rotation = d_camera->rotation(d_transform->position);
	d_camera->updateViewMatrix(d_transform->position);

	// Update camera
	d_transform->rotation = d_camera->rotation(d_transform->position);
	d_camera->updateViewMatrix(d_transform->position);


	// Do some random stuff
	DataMapper<Data::Update> map_update;
	while(map_update.hasNext())
	{
		Entity* e = map_update.nextEntity();
		if(!e->fetchData<Data::Selected>())
		{
			Data::Transform* d_transform = e->fetchData<Data::Transform>();
			Data::Update* d_update = e->fetchData<Data::Update>();
			d_transform->position = d_transform->position + d_update->direction * d_update->speed * SETTINGS()->deltaTime;

			// Apply rotation
			Vector3 v = d_update->rotation * SETTINGS()->deltaTime;
			Matrix m1 = Matrix::CreateFromQuaternion(d_transform->rotation);
			Matrix m2 = Matrix::CreateFromYawPitchRoll(v.x, v.y, v.z);
			m1 = m1 * m2;
			d_transform->rotation = Quaternion::CreateFromRotationMatrix(m1);
		}
	}
}
