#pragma once

#include <string>
#include "EntityPointer.h"
#include "Enums.h"
#include "Manager_Data.h"

class Manager_Entity;

/**
Standard representation of a game object.
*/
class Entity
{
private:
	static Manager_Data* s_manager_data;
	static Manager_Entity* s_manager_entity;

private:
	int m_id;
	int m_uniqueId;
	std::string m_name;
	Enum::EntityType m_type;

public:
	Entity(int p_id, int p_uniqueId = 0);

	/**
	Setup class with with architecture. Must be
	called before use.
	*/
	static void initClass();

	/**
	Called when deleting an Entity. 
	*/
	void clean();

	/**
	Saves the Entity into a batch pointer.
	This makes it safer to access the Entity elsewhere.
	*/
	EntityPointer toPointer();

	/**
	Adds Data to Entity.
	*/
	template<typename T>
	T* addData(Data::Type<T>& p_data)
	{
		return s_manager_data->addData(id(), p_data);
	}

	/**
	Removes Data from Entity
	*/
	template<typename T>
	void removeData()
	{
		int p_batchIndex = Data::Type<T>::classId();
		s_manager_data->removeData<T>(id(), p_batchIndex);
	}

	
	/**
	Fast fetch of data. Almost as fast as DataMapper
	though DataMapper should be preferred.
	Returns a pointer to the Data, or
	NULL if the Data is missing.
	*/
	template<typename T>
	T* fetchData()
	{
		// ClassId maps to batchIndex, i.e. the index 
		// of the batch (array) storing data of type T
		int p_batchIndex = Data::Type<T>::classId();

		// Fetch data from Data Manager
		return s_manager_data->fetchData<T>(id(), p_batchIndex);
	}

	static Entity* findEntity(int p_id);

	void removeEntity();

public:
	int id();
	int uniqueId();
	std::string name();
	void setName(std::string p_name, int p_number);
	Enum::EntityType type();
	void setType(Enum::EntityType type);
};