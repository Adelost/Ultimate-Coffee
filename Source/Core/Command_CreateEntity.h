#pragma once

#include "Command.h"

#include "Enums.h"
#include "Math.h"

class Entity;

class Command_CreateEntity : public Command
{
public:
	struct dataStruct
	{
		bool createCommand; // If false, command acts as remove command
		Enum::EntityType entityType;
		int entityId;
		int entityUniqueId;
		Vector3 position;
		Quaternion rotation;
		Vector3 scale;
		Vector3 color;
	};

	dataStruct m_data;

public:
	Command_CreateEntity(bool create);
	Command_CreateEntity(Entity* e, bool create = true);

	~Command_CreateEntity();

	virtual void doRedo();
	virtual void undo();
	void createEntity();
	void removeEntity();

	virtual void* accessDerivedClassDataStruct();
	virtual int getByteSizeOfDataStruct();

	virtual void loadDataStructFromBytes(char* data);
};