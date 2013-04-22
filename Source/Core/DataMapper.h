#pragma once

#include "Init_DataMapper.h"
#include "Manager_Data.h"

class IDataMapper
{
private:
	Manager_Data* m_data;
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
	std::vector<T>* m_data_list;
	std::vector<int>* m_owner_list;
	int m_index_lastGap;

	int m_index_next;

public:
	DataMapper()
	{
		m_index_next = 0;

		// Map to corresponding Data
		Init_DataMapper init;
		mapToData<T>(&init);
		m_data_list = init.dataList<T>();
		m_owner_list = init.owner_list;
		m_index_lastGap = init.index_lastGap;
	}

	bool hasNext()
	{
		// Step to next Item or until end is reached
		while(m_index_next < m_index_lastGap && (*m_owner_list)[m_index_next] == 0)
		{
			m_index_next++;
		}

		// Returns TRUE if next Item is valid, otherwise end has been reached
		if(m_index_next < m_index_lastGap)
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

	void resetIndex()
	{
		m_index_next = 0;
	}
	
	// Fetch current item, and step to next item 
	T* next()
	{
		m_index_next++;
		return &(*m_data_list)[m_index_next-1];
	}
};