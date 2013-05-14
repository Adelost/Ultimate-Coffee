#include "stdafx.h"
#include "Entity.h"
#include "World.h"
#include "Manager_Entity.h"

Entity::Entity( int p_id, int p_uniqueId )
{
	m_id = p_id;
	m_uniqueId = p_uniqueId;
	m_data = WORLD()->manager_data();

	setName("Nameless", m_uniqueId);
}

void Entity::removeEntity()
{
	WORLD()->removeEntity(this);
}

void Entity::clean()
{
	m_uniqueId = -1;
}

int Entity::id()
{
	return m_id;
}

int Entity::uniqueId()
{
	return m_uniqueId;
}

EntityPointer Entity::toPointer()
{
	EntityPointer ptr;
	ptr.init(this);

	return ptr;
}

Entity* Entity::findEntity( int p_id )
{
	return WORLD()->manager_entity()->entityAt(p_id);
}

std::string Entity::name()
{
	return m_name;
}

void Entity::setName( std::string p_name, int p_number )
{
	m_name = p_name + "_" + Converter::IntToStr(p_number);
}
