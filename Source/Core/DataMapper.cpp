#include "stdafx.h"
#include "DataMapper.h"

#include "World.h"
#include "Manager_Entity.h"

IDataMapper::IDataMapper()
{
	m_data = WORLD()->manager_data();
}

Entity* IDataMapper::entityAt( int p_index )
{
	return WORLD()->manager_entity()->entityAt(p_index);
}

int IDataMapper::getEntityId( Entity* p_entity )
{
	return p_entity->id();
}
