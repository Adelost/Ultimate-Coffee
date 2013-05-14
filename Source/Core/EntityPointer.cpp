#include "stdafx.h"
#include "EntityPointer.h"


EntityPointer::EntityPointer()
{
	m_index = -1;
	// Deleted Entities have a uniqueId of -1 during the
	// "isValid" check. Since we don't want false positives, 
	// our standard value has to be different.
	m_uniqueId = -2;
}

Entity* EntityPointer::operator->()
{
	return asEntity();
}

bool EntityPointer::isValid()
{
	// Make sure index is valid
	if(m_index != -1)
	{
		// Makes sure the Entity we originally pointed
		// to have not been replaced.
		std::vector<Entity>* entity_list = static_cast<std::vector<Entity>*>(s_hostEntity_list);
		int entityUniqueId = (*entity_list)[m_index].uniqueId();

		if(entityUniqueId == m_uniqueId)
			return true;
	}
	return false;
}

void EntityPointer::init( Entity* entity )
{
	m_index = entity->id();
	m_uniqueId = entity->uniqueId();
}

void EntityPointer::initClass( void* hostEntity_list )
{
	s_hostEntity_list = hostEntity_list;
}

Entity* EntityPointer::asEntity()
{
	std::vector<Entity>* entity_list = static_cast<std::vector<Entity>*>(s_hostEntity_list);
	Entity* e = &(*entity_list)[m_index];

	return e;
}

bool EntityPointer::operator==( Entity* e )
{
	return e->uniqueId() == m_uniqueId;
}

void* EntityPointer::s_hostEntity_list = nullptr;