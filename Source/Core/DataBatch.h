#pragma once

#include "IData.h"
#include "Batch.h"
#include "Init_DataMapper.h"

class DataBatch
{
private:
	IBatch* m_batch;
	std::vector<int> m_owner_list;
	// The index where data is stored, corresponding
	// to each EntityId
	std::vector<int> m_dataIndexFromEntityId_list;

public:
	DataBatch(IBatch* p_batch)
	{
		m_batch = p_batch;
	}

	/**
	// Fetches the dataIndex of a certain data (Batch)
	// belonging to the entity. Returns NULL if the 
	// Data is missing.
	*/
	int dataIndexFromEntityId(int p_entityId)
	{
		if(p_entityId >= (int)m_dataIndexFromEntityId_list.size())
			return -1;

		return m_dataIndexFromEntityId_list[p_entityId];
	}

	template<typename T>
	void addData(int p_entityId, Data::Type<T>& p_data)
	{
		// Delete previous Data, if any
		int prev_dataId = dataIndexFromEntityId(p_entityId);
		if(prev_dataId != -1)
			removeData<T>(p_entityId);

		// Put data inside Batch and retrieve the 
		// index where the data was stored
		Batch<T>* batch = static_cast<Batch<T>*>(m_batch);
		int dataIndex = batch->addItem(static_cast<T&>(p_data));

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
	}

	template<typename T>
	T* fetchData(int p_entityId)
	{
		// Fetch the corresponding batch and convert into
		// it's subclass.
		Batch<T>* batch = static_cast<Batch<T>*>(m_batch);

		// Fetch item from Batch
		int dataIndex = dataIndexFromEntityId(p_entityId);

		// Return NULL if Data is missing
		if(dataIndex == -1)
			return NULL;

		return batch->itemAt(dataIndex);
	}

	template<typename T>
	void removeData(int p_entityId)
	{
		Batch<T>* batch = static_cast<Batch<T>*>(m_batch);

		int dataIndex = dataIndexFromEntityId(p_entityId);

		if(dataIndex != -1)
			batch->removeItemAt(dataIndex);
	}

	template<typename T>
	void mapToData(Init_DataMapper* p_init)
	{
		Batch<T>* batch = static_cast<Batch<T>*>(m_batch);
		batch->mapToData(p_init);
		p_init->owner_list = &m_owner_list;
	}
};