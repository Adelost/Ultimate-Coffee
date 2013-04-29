#include "stdafx.h"
#include "World.h"
#include "Manager_Data.h"
#include "Manager_Systems.h"
#include "Manager_Entity.h"
#include "ISystem.h"

void World::addSystem( ISystem* p_system )
{
	m_manager_systems->addSystem(p_system);
}

Manager_Systems* World::manager_systems()
{
	return m_manager_systems;
}

Manager_Data* World::manager_data()
{
	return m_manager_data;
}

World* World::instance()
{
	static World instance;
	return &instance;
}

World::World()
{
	m_manager_systems = new Manager_Systems();
	m_manager_data = new Manager_Data();
	m_manager_entity = new Manager_Entity();
	settings = new Settings();
}

World::~World()
{
	delete m_manager_systems;
	delete m_manager_data;
	delete m_manager_entity;
	delete settings;
}

void World::update()
{
	m_manager_systems->update();
}

Entity* World::createEntity()
{
	return m_manager_entity->create();
}

