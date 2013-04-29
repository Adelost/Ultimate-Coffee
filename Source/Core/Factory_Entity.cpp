#include "stdafx.h"
#include "Factory_Entity.h"

#include "World.h"
#include "Manager_Entity.h"
#include "Data.h"


Factory_Entity::Factory_Entity()
{
	m_manager_entity = WORLD()->manager_entity();
}

Entity* Factory_Entity::createEntity( EntityType p_type )
{
	Entity* e = m_manager_entity->create();

	if(p_type == ENTITY_EMPTY)
	{
		e->addData(Data::Transform());
		//e->addData(Data::Render());
	}

	if(p_type == ENTITY_CUBE)
	{
		e->addData(Data::Transform());
		//e->addData(Data::Render());
	}

	return e;
}
