#pragma once

#include "Util.h"
#include "IData.h"
#include "ISystem.h"
#include "Batch.h"
#include "Entity.h"

class World;


class Manager_Entity
{
private:
	Batch<Entity> entity_list;
	World* m_world;

public:
	Manager_Entity()
	{
	}

	Entity* create()
	{
		int entityId = entity_list.nextAvailableIndex();
		entity_list.addItem(Entity(entityId));

		return entity_list.itemAt(entityId);
	}

	void remove(Entity* entity)
	{
		entity_list.removeItemAt(entity->id());
	}
};