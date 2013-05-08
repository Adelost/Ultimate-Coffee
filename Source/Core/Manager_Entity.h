#pragma once

#include "Batch.h"

#include "Entity.h"
class World;

class Manager_Entity
{
private:
	Batch<Entity> m_entity_list;
	int nextUniqueIndex;

public:
	Manager_Entity();
	Entity* entityAt(int p_index);
	Entity* create();
	void remove(Entity* p_entity);
};