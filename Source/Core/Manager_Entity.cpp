#include "stdafx.h"
#include "Manager_Entity.h"
#include "Data.h"
#include "EntityPointer.h"

Manager_Entity::Manager_Entity()
{
	m_nextUniqueId = 0;
	m_reservedUniqueId = -1;
	m_reservedId = -1;
}

Entity* Manager_Entity::entityAt( int p_index )
{
	Entity* e = m_entity_list.itemAt(p_index);

	return e;
}

Entity* Manager_Entity::create()
{
	int entityId = m_entity_list.nextAvailableIndex();
	int uniqueId;
	if(m_reservedUniqueId != -1)
	{
		uniqueId = m_reservedUniqueId;
		m_reservedUniqueId = -1;
	}
	else
	{
		 uniqueId = m_nextUniqueId;
		 m_nextUniqueId++;
	}	

	// HACK: Overwrite id storage place if we are undoing
	if(m_reservedId != -1)
	{
		entityId = m_reservedId;
		m_entity_list.overwrite(Entity(entityId, uniqueId), m_reservedId);
		m_reservedId = -1;
	}
	else
	{
		m_entity_list.addItem(Entity(entityId, uniqueId));
	}

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

void Manager_Entity::clear()
{
	for(int i=0; i<m_entity_list.indexCount(); i++)
	{
		Entity* e = entityAt(i);
		if(e->uniqueId() != -1)
		{
			e->removeEntity();
		}
	}
}

void Manager_Entity::init()
{
	Entity::initClass();
	EntityPointer::initClass(m_entity_list.itemList());
}