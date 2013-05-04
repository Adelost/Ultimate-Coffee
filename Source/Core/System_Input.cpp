#include "stdafx.h"
#include "System_Input.h"
#include "Data.h"
#include "Entity.h"
#include "World.h"

void System::Input::update()
{
	//Entity entity_camera = CAMERA_ENTITY();
	//Data::Transform* d_transform = entity_camera.fetchData<Data::Transform>();
	//Data::Camera* d_camera = entity_camera.fetchData<Data::Camera>();

	//float delta = SETTINGS()->deltaTime;
	//float strafe = 0.0f;
	//float walk = 0.0f;

	//if(SETTINGS->button.)
	//{
	//	walk += delta;
	//}
	//if(e->key() == Qt::Key_A)
	//{
	//	strafe -= delta;
	//}
	//if(e->key() == Qt::Key_S)
	//{
	//	walk -= delta;
	//}
	//if(e->key() == Qt::Key_D)
	//{
	//	strafe += delta;
	//}

	//// Rotate camera
	//d_camera->strafe(d_transform->position, strafe);
	//d_camera->walk(d_transform->position, walk);
	//d_camera->updateViewMatrix(d_transform->position);
}
