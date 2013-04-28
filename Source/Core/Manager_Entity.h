#pragma once

#include "stdafx.h"//check
#include "IData.h"
#include "ISystem.h"
#include "Batch.h"
#include "Entity.h"
class World;


class Manager_Entity
{
private:
	Batch<Entity> m_entity_list;
	World* m_world;

public:
	Manager_Entity()
	{
	}

	Entity* create()
	{
		int entityId = m_entity_list.nextAvailableIndex();
		m_entity_list.addItem(Entity(entityId));

		return m_entity_list.itemAt(entityId);
	}

	void remove(Entity* entity)
	{
		m_entity_list.removeItemAt(entity->id());
	}
};