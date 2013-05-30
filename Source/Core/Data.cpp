#include "stdafx.h"
#include "Data.h"

#include "xnacollision.h"

#include "DataMapper.h"
#include "Entity.h"
#include "World.h"

EntityPointer Data::Selected::lastSelected;


Entity* Data::Bounding::intersect( const Ray& ray )
{
	Entity* out = nullptr;
	float hitDistance = FLT_MAX;

	DataMapper<Data::Bounding> map_bounding;
	while(map_bounding.hasNext())
	{
		Entity* entity = map_bounding.nextEntity();
		Data::Bounding* d_bounding  = entity->fetchData<Data::Bounding>();

		float d;
		if(d_bounding->intersect(entity, ray, &d))
		{
			if(d < hitDistance)
			{
				out = entity;
				hitDistance = d;
			}
		}	
	}

	return out;
}

bool Data::Bounding::intersect( Entity* entity, const Ray& ray, float* distance)
{
	// Translate Ray to local space
	Data::Transform* d_transform  = entity->fetchData<Data::Transform>();
	Matrix m = d_transform->toWorldMatrix();
	m = m.Invert();
	Ray r;
	r.position = Vector3::Transform(ray.position, m);
	r.direction = Vector3::TransformNormal(ray.direction, m);
	r.direction.Normalize();


	// Perform intersection test
	bool out = false;
	BoundingSphere sphere(Vector3(0.0f), 1.0f);
	if(r.Intersects(sphere, *distance))
	{
		BoundingBox b(Vector3(0.0f), Vector3(0.5f));
		out = r.Intersects(b, *distance);
	}

	return out;
}

void Data::Bounding::intersect( const BoundingFrustum& frustum, std::vector<Entity*>* entity_list )
{
	DataMapper<Data::Bounding> map_bounding;
	while(map_bounding.hasNext())
	{
		Entity* entity = map_bounding.nextEntity();
		Data::Bounding* d_bounding  = entity->fetchData<Data::Bounding>();
		Data::Transform* d_transform  = entity->fetchData<Data::Transform>();
		
		// Check intersection
		BoundingSphere sphere(d_transform->position, 1.0f);
		if(sphere.Intersects(frustum))
			entity_list->push_back(entity);
	}
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

void Data::AddedToClipboard::clearClipboard()
{
	DataMapper<Data::AddedToClipboard> map_clipboard;
	while(map_clipboard.hasNext())
	{
		Entity* e = map_clipboard.nextEntity();
		e->removeData<Data::AddedToClipboard>();
	}

	// All selection was cleared, no entity is selected
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

Matrix Data::Transform::toWorldMatrix()
{
	Matrix mat_scale = Matrix::CreateScale(scale);
	Matrix mat_rot = Matrix::CreateFromQuaternion(rotation);
	Matrix mat_trans = Matrix::CreateTranslation(position);

	//Matrix m = mat_rot * mat_scale * mat_trans; //Matrix m = mat_scale*toRotPosMatrix();
	Matrix m = mat_scale * mat_rot * mat_trans;
	return m;
}

Matrix Data::Transform::toRotPosMatrix()
{
	Matrix mat_pos = Matrix::CreateTranslation(position);
	Matrix mat_rot = Matrix::CreateFromQuaternion(rotation);

	Matrix m = mat_rot*mat_pos;
	return m;
}

Matrix Data::Transform::toPosMatrix()
{
	Matrix m = Matrix::CreateTranslation(position);

	return m;
}

Matrix Data::Transform::toRotMatrix()
{
	Matrix m = Matrix::CreateFromQuaternion(rotation);

	return m;
}

Data::Movement_Floating::Movement_Floating()
{
	mass = 50.0f;
	velocity = Math::randomVector(-0.1f, 0.1f);

	// Rotation
	rotation = velocity = Math::randomVector(-0.1f, 0.1f);;
}
bool Data::Movement_Floating::targetCamera = false;

Data::Render::Manager Data::Render::manager;

Data::Render::Render( Entity* entity, int meshId )
{
	invisible = false;
	this->owner = entity->toPointer();
	setMesh(meshId);
}

void Data::Render::recoverFromCloning( Entity* owner )
{
	this->owner = owner->toPointer();
	mesh.index = -1;
	setMesh(mesh.id);
}


void Data::ZoomTo::zoomTo( Entity* e )
{
	// Fetch camera
	Entity* entity_camera = CAMERA_ENTITY().asEntity();

	// HACK: To tired to say why
	if(entity_camera->id() == e->id())
		return;


	Data::ZoomTo d_zoomTo;
	d_zoomTo.target = e->toPointer();
	Data::Transform* d_cameraTransform = entity_camera->fetchData<Data::Transform>();
	Data::Transform* d_clickedEntityTransform = d_zoomTo.target->fetchData<Data::Transform>();
	Data::Camera* d_camera = entity_camera->fetchData<Data::Camera>();
	Data::Render* d_render = d_zoomTo.target->fetchData<Data::Render>();

	if(d_camera != nullptr)
	{
		d_zoomTo.originLook = d_camera->getLookVector();
	}
	else
	{
		d_zoomTo.originLook = Vector3(0.0f, 0.0f, 1.0f);
	}

	float largestMeshSide = 1.0f;
	if(d_render != nullptr)
	{
		largestMeshSide = Data::Render::manager.buffer_list[d_render->mesh.id].largestValue;
	}

	if(d_cameraTransform != nullptr && d_clickedEntityTransform != nullptr)
	{
		d_zoomTo.rotationLerpT = 0.0f;
		d_zoomTo.distanceFromTargetToStopAt = 2.5f * largestMeshSide;
		d_zoomTo.delay = 1.0f;
		float distance = Vector3::Distance(d_clickedEntityTransform->position, d_cameraTransform->position) - d_zoomTo.distanceFromTargetToStopAt;
		d_zoomTo.speed = distance / d_zoomTo.delay;
		d_zoomTo.speed = max(d_zoomTo.speed, d_zoomTo.delay);
		entity_camera->addData(d_zoomTo);
	}
}
