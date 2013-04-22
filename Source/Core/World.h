#pragma once

#include "Data.h"
#include "System.h"
#include "Entity.h"
#include "Settings.h"

class Manager_Systems;
class Manager_Data;
class Manager_Entity;


class World
{
private:
	Manager_Systems* m_manager_systems;
	Manager_Data* m_manager_data;
	Manager_Entity* m_manager_entity;
	
public:
	Settings* settings;

private:
	World();

public:
	static World* instance();
	~World();
	void update();
	
	void addSystem(ISystem* p_system);

public:
	Manager_Data* manager_data();
	Manager_Systems* manager_systems();
};

// Access World
#define WORLD()	\
	World::instance()

// Access World
#define SETTINGS()	\
	World::instance()->settings