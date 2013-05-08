#include "stdafx.h"
#include "Manager_Entity.h"

#include "EntityPointer.h"

Manager_Entity::Manager_Entity()
{
	nextUniqueIndex = 0;
	EntityPointer::initClass(m_entity_list.itemList());
}

Entity* Manager_Entity::entityAt( int p_index )
{
	Entity* e = m_entity_list.itemAt(p_index);

	return e;
}

Entity* Manager_Entity::create()
{
	int entityId = m_entity_list.nextAvailableIndex();
	m_entity_list.addItem(Entity(entityId, nextUniqueIndex));
	nextUniqueIndex++;

	return m_entity_list.itemAt(entityId);
}

void Manager_Entity::remove( Entity* p_entity )
{
	p_entity->clean();
	m_entity_list.removeItemAt(p_entity->id());
}
