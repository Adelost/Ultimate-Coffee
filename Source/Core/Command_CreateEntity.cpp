#include "stdafx.h"
#include "Command_CreateEntity.h"

#include "World.h"
#include "Data.h"
#include "Factory_Entity.h"
#include "Manager_Entity.h"

Command_CreateEntity::~Command_CreateEntity()
{
}

void Command_CreateEntity::doRedo()
{
	if(m_data.createCommand)
		createEntity();
	else
		removeEntity();
}

void Command_CreateEntity::undo()
{
	if(m_data.createCommand)
		removeEntity();
	else
		createEntity();
}

void* Command_CreateEntity::accessDerivedClassDataStruct()
{
	return reinterpret_cast<void*>(&m_data);
}

int Command_CreateEntity::getByteSizeOfDataStruct()
{
	return sizeof(m_data);
}

void Command_CreateEntity::loadDataStructFromBytes( char* data )
{
	m_data = *reinterpret_cast<dataStruct*>(&data);
}

void Command_CreateEntity::createEntity()
{
	// Create Entity
	int id = m_data.entityUniqueId;
	WORLD()->manager_entity()->reserveId(id);
	Entity* e = FACTORY_ENTITY()->createEntity(m_data.entityType);
	m_data.entityId = e->id();

	// Assign spatial data
	Data::Transform* d_transform = e->fetchData<Data::Transform>();
	if(d_transform)
	{
		d_transform->position = m_data.position;
		d_transform->rotation = m_data.rotation;
		d_transform->scale = m_data.scale;
	}
}

void Command_CreateEntity::removeEntity()
{
	int id = m_data.entityUniqueId;
	Entity* e = Entity::findEntity(m_data.entityId);
	e->removeEntity();
}

Command_CreateEntity::Command_CreateEntity( Entity* e, bool create )
{
	m_data.createCommand = create;
	if(create)
		setType(Enum::CommandType::CREATE_ENTITY);
	else
		setType(Enum::CommandType::REMOVE_ENTITY);
	std::string name;
	if(create)
		name = "New " + e->name();
	else
		name = "Remove " + e->name();
	setName(name);

	Data::Transform* d_transform = e->fetchData<Data::Transform>();
	m_data.entityId = e->id();
	m_data.entityUniqueId = e->uniqueId();
	m_data.entityType = e->type();
	m_data.position = d_transform->position;
	m_data.rotation = d_transform->rotation;
	m_data.scale = d_transform->scale;
}

Command_CreateEntity::Command_CreateEntity( bool create )
{
	if(create)
		setType(Enum::CommandType::CREATE_ENTITY);
	else
		setType(Enum::CommandType::REMOVE_ENTITY);
}
