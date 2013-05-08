#include "stdafx.h"
#include "System_Input.h"

#include "Data.h"
#include "World.h"

void System::Input::update()
{
	Entity* entity_camera = CAMERA_ENTITY().asEntity();
	
	Data::Transform* d_transform = entity_camera->fetchData<Data::Transform>();
	Data::Camera* d_camera = entity_camera->fetchData<Data::Camera>();

	float delta = SETTINGS()->deltaTime * 15.0f;
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
	d_camera->updateViewMatrix(d_transform->position);
}
