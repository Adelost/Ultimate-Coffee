#pragma once

#include "Batch.h"

#include "Entity.h"
class World;

class Manager_Entity
{
private:
	Batch<Entity> m_entity_list;
	int m_nextUniqueId;
	int m_reservedUniqueId;
	int m_reservedId;

public:
	Manager_Entity();
	void init();
	Entity* entityAt(int p_index);
	Entity* create();
	void remove(Entity* p_entity);
	/**
	Allows Entities to retain their UniqueId
	if they need to be readded to the world.
	Used with caution.
	*/
	void reserveUniqueId(int id)
	{
		m_reservedUniqueId = id;
	}
	void reserveId(int id)
	{
		m_reservedId = id;
	}
	void clear();
};