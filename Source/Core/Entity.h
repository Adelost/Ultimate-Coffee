#pragma once

#include "Manager_Data.h"
#include "EntityPointer.h"

class World;

/**
Standard representation of a game object.
*/
class Entity
{
private:
	int m_id;
	int m_uniqueId;
	Manager_Data* m_data;

public:
	Entity(int p_id, int p_uniqueId = 0);

	/**
	Called when deleting an Entity. 
	*/
	void clean();

	int id();

	int uniqueId();

	/**
	Saves the Entity into a batch pointer.
	This makes it safer to access the Entity elsewhere.
	*/
	EntityPointer asPointer();

	/**
	Adds Data to Entity.
	*/
	template<typename T>
	T* addData(Data::Type<T>& p_data)
	{
		return m_data->addData(id(), p_data);
	}

	/**
	Removes Data from Entity
	*/
	template<typename T>
	void removeData()
	{
		int p_batchIndex = Data::Type<T>::classId();
		m_data->removeData<T>(id(), p_batchIndex);
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
		return m_data->fetchData<T>(id(), p_batchIndex);
	}

	
	static Entity* findEntity(int p_id);

	void removeEntity();
};