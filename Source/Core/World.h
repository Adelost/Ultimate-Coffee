#pragma once

class Manager_Systems;
class Manager_Data;
class Manager_Entity;
class Settings;
class Entity;
class ISystem;
class Factory_Entity;

class World
{
private:
	Manager_Systems* m_manager_systems;
	Manager_Data* m_manager_data;
	Manager_Entity* m_manager_entity;
	Factory_Entity* m_factory_entity;
	
public:
	Settings* settings;

private:
	World();

public:
	static World* instance();
	~World();
	void update();
	
	void addSystem(ISystem* p_system);
	Entity* createEntity();

public:
	Manager_Data* manager_data();
	Manager_Systems* manager_systems();
	Manager_Entity* manager_entity();
	Factory_Entity* factory_entity();
};

// Access World
#define WORLD()	\
	World::instance()

// Access World Settings
#define SETTINGS()	\
	World::instance()->settings

// Access World Settings
#define FACTORY_ENTITY()	\
	World::instance()->factory_entity()

// Access World Settings
#define CAMERA_ENTITY()	\
	World::instance()->settings->entity_camera