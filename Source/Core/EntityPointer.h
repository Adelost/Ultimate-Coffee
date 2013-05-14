#pragma once

class Entity;

class EntityPointer
{
private:
	static void* s_hostEntity_list;
	int m_index;
	int m_uniqueId;

public:
	EntityPointer();
	void init(Entity* entity);
	static void initClass(void* hostEntity_list);

	/**
	Makes sure the Entity we originally pointed to have not been replaced.
	*/
	bool isValid();

	/**
	Get Entity. Make sure to call isValid before, to make sure the Entity have not changed.
	*/
	Entity* operator->();

	Entity* asEntity();

	void invalidate()
	{
		m_index = -1;
	}

	bool operator==(Entity* e);
};