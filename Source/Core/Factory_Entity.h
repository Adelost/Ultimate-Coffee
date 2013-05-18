#pragma once

#include "Enums.h"

class Manager_Entity;
class Entity;
class Command;


class Factory_Entity
{
private:
	Manager_Entity* m_manager_entity;

public:
	Factory_Entity();
	Entity* createEntity(Enum::EntityType type, bool addToHistory = false);
};