#include "stdafx.h"
#include "Entity.h"
#include "World.h"
#include "Manager_Entity.h"
#include "Data.h"

Entity::Entity( int p_id, int p_uniqueId )
{
	m_id = p_id;
	m_uniqueId = p_uniqueId;
	m_type = Enum::Entity_Empty;
	hierarchyRow = -1;
}

void Entity::removeEntity()
{
	s_manager_data->removeEntity(id());
	s_manager_entity->remove(this);
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
	Manager_Entity* s_manager_entity = WORLD()->manager_entity();
	return s_manager_entity->entityAt(p_id);
}

// std::string Entity::name()
// {
// 	return m_name;
// }
// 
// void Entity::setName( std::string p_name, int p_number )
// {
// 	m_name = p_name + "_" + Converter::IntToStr(p_number);
// }

Enum::EntityType Entity::type()
{
	return m_type;
}

void Entity::setType( Enum::EntityType type )
{
	m_type = type;
}

void Entity::initClass()
{
	s_manager_data = WORLD()->manager_data();
	s_manager_entity = WORLD()->manager_entity();
}

Entity* Entity::clone()
{
	EntityPointer e = this->toPointer();
	Entity* clone = s_manager_entity->create();

	// Copy Entity
	// HACK: Is this a shallow copy?
	// Be careful now.
	//*clone = *e.asEntity();
	clone->setType(e->type());

	// Copy Data
	s_manager_data->cloneData(e->id(), clone->id());
	clone->addData(Data::Created());

	return clone;
}

std::string Entity::name()
{
	std::string name = typeName();
	name += "_" + Converter::IntToStr(id());

	return name;
}

std::string Entity::typeName()
{
	return EntityTypeToString(type());
}


Manager_Entity* Entity::s_manager_entity;

Manager_Data* Entity::s_manager_data;
