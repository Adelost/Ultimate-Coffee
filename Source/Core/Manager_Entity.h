#pragma once

#include "Batch.h"
#include "Entity.h"
class World;

class Manager_Entity
{
private:
	Batch<Entity> m_entity_list;

public:
	Manager_Entity()
	{
	}

	Entity* entityAt(int p_index)
	{
		Entity* e = m_entity_list.itemAt(p_index);

		return e;
	}

	Entity* create()
	{
		int entityId = m_entity_list.nextAvailableIndex();
		m_entity_list.addItem(Entity(entityId));

		return m_entity_list.itemAt(entityId);
	}

	void remove(Entity* p_entity)
	{
		m_entity_list.removeItemAt(p_entity->id());
	}
};