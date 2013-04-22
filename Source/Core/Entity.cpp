#include "Entity.h"

#include "World.h"


Entity::Entity( int p_id )
{
	m_id = p_id;
	m_data = WORLD()->manager_data();
}

void Entity::removeEntity()
{
	//WORLD()->
}
