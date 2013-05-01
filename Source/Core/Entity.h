#pragma once


class World;

// Standard representation of a game object
class Entity
{
private:
	int m_id;
	Manager_Data* m_data;

public:
	Entity(int p_id);

	int id()
	{
		return m_id;
	}

	// Adds Data to entity
	template<typename T>
	void addData(Data::Type<T>& p_data)
	{
		m_data->addData(id(), p_data);
	}

	// Removes Data from entity
	template<typename T>
	void removeData()
	{
		int p_batchIndex = Data::Type<T>::classId();
		m_data->removeData<T>(id(), p_batchIndex);
	}

	// Fast fetch of data. Almost as fast as DataMapper
	// though DataMapper should be preferred.
	// Returns a pointer to the Data, or
	// NULL if the Data is missing.
	template<typename T>
	T* fetchData()
	{
		// ClassId maps to batchIndex, i.e. the index 
		// of the batch (array) storing data of type T
		int p_batchIndex = Data::Type<T>::classId();

		// Fetch data from Data Manager
		return m_data->fetchData<T>(id(), p_batchIndex);
	}

	void removeEntity();

private:
};