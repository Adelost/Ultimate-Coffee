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
	m_data = *reinterpret_cast<dataStruct*>(data);
}

void Command_CreateEntity::createEntity()
{
	// Create Entity
	int uniqueId = m_data.entityUniqueId;
	int id = m_data.entityId;
	WORLD()->manager_entity()->reserveUniqueId(uniqueId);
	WORLD()->manager_entity()->reserveId(id);
	if(m_data.entityType == Enum::Entity_Empty)
	{
		int t;
		t = 0;
	}
	Entity* e = FACTORY_ENTITY()->createEntity(m_data.entityType);

	// Assign spatial data
	Data::Transform* d_transform = e->fetchData<Data::Transform>();
	if(d_transform)
	{
		d_transform->position = m_data.position;
		d_transform->rotation = m_data.rotation;
		d_transform->scale = m_data.scale;
	}

	// Render
	Data::Render* d_render = e->fetchData<Data::Render>();
	if(d_render)
	{
		d_render->mesh.color = m_data.color;
		d_render->setMesh(m_data.mesh);
	}
}

void Command_CreateEntity::removeEntity()
{
	if(m_data.entityType == Enum::Entity_Empty)
	{
		int t;
		t = 0;
	}

	int id = m_data.entityUniqueId;
	Entity* e = Entity::findEntity(m_data.entityId);

	// Render
	Data::Transform* d_transform = e->fetchData<Data::Transform>();
	m_data.entityId = e->id();
	m_data.entityUniqueId = e->uniqueId();
	m_data.entityType = e->type();
	m_data.position = d_transform->position;
	m_data.rotation = d_transform->rotation;
	m_data.scale = d_transform->scale;

	// Render
	Data::Render* d_render = e->fetchData<Data::Render>();
	if(d_render)
	{
		m_data.color = d_render->mesh.color;
		m_data.mesh = d_render->mesh.id;
	}

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
	
	//check. If an entity has a name that needs to be saved to file, put it in the data struct of the command (Henrik, 2013-05-18, 14.34)
	//if(create)
	//	name = "New " + e->name();
	//else
	//	name = "Remove " + e->name();
	//setName(name);

	Data::Transform* d_transform = e->fetchData<Data::Transform>();
	m_data.entityId = e->id();
	m_data.entityUniqueId = e->uniqueId();
	m_data.entityType = e->type();
	m_data.position = d_transform->position;
	m_data.rotation = d_transform->rotation;
	m_data.scale = d_transform->scale;

	// Render
	Data::Render* d_render = e->fetchData<Data::Render>();
	if(d_render)
	{
		m_data.color = d_render->mesh.color;
		m_data.mesh = d_render->mesh.id;
	}

	if(m_data.entityType == Enum::Entity_Empty)
	{
		int t;
		t = 0;
	}
}

Command_CreateEntity::Command_CreateEntity( bool create )
{
	if(create)
		setType(Enum::CommandType::CREATE_ENTITY);
	else
		setType(Enum::CommandType::REMOVE_ENTITY);
}
