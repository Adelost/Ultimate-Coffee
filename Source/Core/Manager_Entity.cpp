#include "stdafx.h"
#include "Manager_Entity.h"
#include "Data.h"
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

	Entity* e = m_entity_list.itemAt(entityId);
	e->addData(Data::Created());

	return e;
}

void Manager_Entity::remove( Entity* e )
{
	e->clean();
	m_entity_list.removeItemAt(e->id());
	e->addData(Data::Deleted());
}
