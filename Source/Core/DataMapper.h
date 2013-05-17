#pragma once

#include "Init_DataMapper.h"
#include "Manager_Data.h"

class Entity;
class IDataMapper
{
private:
	Manager_Data* m_data;

protected:
	Entity* entityAt(int p_index);
	int getEntityId(Entity* p_entity);

public:
	IDataMapper();
	
protected:
	template<typename T>
	void mapToData(Init_DataMapper* p_init)
	{
		m_data->mapToData<T>(p_init);
	}
};

template<typename T>
class DataMapper : public IDataMapper
{
private:
	int m_index_next;
	int* m_dataCount;
	int* m_index_lastGap;
	std::vector<T>* m_data_list;
	std::vector<int>* m_owner_list;

public:
	DataMapper()
	{
		m_index_next = 0;

		// Map to corresponding Data
		Init_DataMapper init;
		mapToData<T>(&init);
		m_data_list = init.dataList<T>();
		m_index_lastGap = init.index_lastGap;
		m_dataCount = init.m_dataCount;
		m_owner_list = init.owner_list;
	}

	bool hasNext()
	{
		// Step to next Item or until end is reached
		while(m_index_next < *m_index_lastGap && (*m_owner_list)[m_index_next] == -1)
		{
			m_index_next++;
		}

		// Returns TRUE if next Item is valid, otherwise end has been reached
		if(m_index_next < *m_index_lastGap)
		{
			return true;
		}
		else
		{
			// Reset index
			resetIndex();
			return false;
		}
	}

	int currentIndex()
	{
		int indexCurrent = m_index_next-1;
		return indexCurrent;
	}

	void resetIndex()
	{
		m_index_next = 0;
	}

	int dataCount()
	{
		return *m_dataCount;
	}

	bool isDeleted(int index)
	{
		return (*m_owner_list)[index] == -1;
	}

	int indexCount()
	{
		return *m_index_lastGap;
	}
	
	/**
	Fetch current item, and step to next item 
	*/
	T* next()
	{
		m_index_next++;
		return &(*m_data_list)[currentIndex()];
	}

	/**
	Fetch the Entity holding the current item, and step to next item 
	*/
	Entity* nextEntity()
	{
		m_index_next++;
		return entityAt((*m_owner_list)[currentIndex()]);
	}

	/**
	Return the Entity holding the Data at the current index
	*/
	Entity* fetchEntity()
	{
		return entityAt(currentIndex());
	}
};