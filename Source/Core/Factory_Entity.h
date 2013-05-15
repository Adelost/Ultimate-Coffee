#pragma once


class Manager_Entity;
class Entity;

enum EntityType
{
	ENTITY_EMPTY,
	ENTITY_SKY,
	ENTITY_CUBE,
	ENTITY_CAMERA,

	ENTITY_END
};

class Factory_Entity
{
private:
	Manager_Entity* m_manager_entity;

public:
	Factory_Entity();
	Entity* createEntity(EntityType p_type);
};