#pragma once

#include "Batch.h" //check, explain this
#include "Entity.h" //check, explain this
class World;

class Manager_Entity
{
private:
	Batch<Entity> m_entity_list;

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