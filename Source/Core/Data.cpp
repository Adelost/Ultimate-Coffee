#include "stdafx.h"
#include "Data.h"

#include "xnacollision.h"

#include "DataMapper.h"
#include "Entity.h"

EntityPointer Data::Selected::pivot;
//
//bool Data::Bounding::intersectRay(Entity* entity, Vector3& origin, Vector3& direction )
//{
//	entity->fetchData<Data::Transform>()
//	DataMapper<Data::Bounding> map_bounding;
//
//	Entity* pickedEntity;
//	Vector3 ray;
//
//
//	
//
//	DataMapper<Data::Bounding> map_bounding;
//	while(map_bounding.hasNext())
//	{
//		Data::Bounding* d_bounding = map_bounding.next();
//		if(d_bounding->intersect(ray)
//		{
//			break;
//		}
//	}
//
//
//	XNA::IntersectRayTriangle(origin,direction, Vector2(x), Vector2(z), Vector2(y));
//}

bool Data::Bounding::intersectRay( Entity* entity, Vector3& origin, Vector3& direction )
{
	/*Data::Transform* d_transform  = entity->fetchData<Data::Transform>();
	XNA::Sphere boundingShape = {d_transform->position, 1.0f};

	float out_d;
	return XNA::IntersectRaySphere(origin, direction, &boundingShape, &out_d);*/
	return false;
}
