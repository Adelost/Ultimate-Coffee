#pragma once

#include "IData.h"
#include "Batch.h"
#include "Init_DataMapper.h"

class IDataBatch
{
public:
	virtual ~IDataBatch(){}
};

template<typename T>
class DataBatch : public IDataBatch
{
private:
	Batch<T> m_batch;
	std::vector<int> m_owner_list;
	/**
	The index where data is stored, corresponding
	to each EntityId
	*/
	std::vector<int> m_dataIndexFromEntityId_list;
	
public:
	/**
	Fetches the dataIndex of a certain data (Batch)
	belonging to the entity. Returns -1 if the 
	Data is missing.
	*/
	int dataIndexFromEntityId(int p_entityId)
	{
		if(p_entityId >= (int)m_dataIndexFromEntityId_list.size())
			return -1;

		return m_dataIndexFromEntityId_list[p_entityId];
	}

	T* addData(int p_entityId, Data::Type<T>& p_data)
	{
		// Delete previous Data, if any
		int prev_dataId = dataIndexFromEntityId(p_entityId);
		if(prev_dataId != -1)
			removeData(p_entityId);

		// Put data inside Batch and retrieve the 
		// index where the data was stored
		T& data = (T&)p_data;
		int dataIndex = m_batch.addItem(data);

		// Save entity as owner to the added Data, 
		// if needed, make room for the new index
		while(dataIndex >= (int)m_owner_list.size())
		{
			m_owner_list.push_back(-1);
		}
		m_owner_list.at(dataIndex) = p_entityId;

		// Save information to make it faster to access data through
		// "entityId" next time
		// NOTE: Since each "index" maps to an Entity, and since all
		// Entities won't have all "data", we will end up with a lot
		// of empty spaces. To ameliorate this, we only increase size
		// as much as necessary. 
		while(p_entityId >= (int)m_dataIndexFromEntityId_list.size())
		{
			m_dataIndexFromEntityId_list.push_back(-1);
		}
		m_dataIndexFromEntityId_list[p_entityId] = dataIndex;

		return m_batch.itemAt(dataIndex);
	}

	T* fetchData(int p_entityId)
	{
		// Fetch item from Batch
		int dataIndex = dataIndexFromEntityId(p_entityId);

		// Return NULL if Data is missing
		if(dataIndex == -1)
			return nullptr;

		return m_batch.itemAt(dataIndex);
	}

	void removeData(int p_entityId)
	{
		int dataIndex = dataIndexFromEntityId(p_entityId);

		if(dataIndex != -1)
			m_batch.removeItemAt(dataIndex);
	}

	void mapToData(Init_DataMapper* p_init)
	{
		p_init->setDataList(m_batch.itemList());
		p_init->index_lastGap = m_batch.lastGap();
		p_init->owner_list = &m_owner_list;
	}
};