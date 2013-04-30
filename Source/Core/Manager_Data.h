#pragma once

#include "DataBatch.h"


class Manager_Data
{
private:
	std::vector<IDataBatch*> m_dataBatch_list;

public:
	~Manager_Data()
	{
		for(int i=0; i<(int)m_dataBatch_list.size(); i++)
			delete m_dataBatch_list[i];
	}

	template<typename T>
	void addData(int p_entityId, Data::Type<T>& p_data)
	{
		int batchIndex = Data::Type<T>::classId();

		// The first time a new Data is added
		// a unique id is given, corresponding to
		// it's storage index, a Batch is also created
		// to store corresponding variables in
		if(batchIndex == -1)
		{
			batchIndex = addBatch<T>();
		}

		DataBatch<T>* dataBatch = (DataBatch<T>*)m_dataBatch_list[batchIndex];
		dataBatch->addData(p_entityId, p_data);
	}

	template<typename T>
	T* fetchData(int p_entityId, int p_batchIndex)
	{
		// Return NULL if Data is missing
		if(p_batchIndex == -1)
			return NULL;

		return m_dataBatch_list[p_batchIndex].fetchData<T>(p_entityId);
	}

	template<typename T>
	void removeData(int p_entityId, int p_batchIndex)
	{
		if(p_batchIndex != -1)
		{
			DataBatch<T>* dataBatch = (DataBatch<T>*)m_dataBatch_list[batchIndex];
			dataBatch->removeData<T>(p_entityId);
		}
	}

	template<typename T>
	int addBatch()
	{
		// Give class an id
		int batchIndex = m_dataBatch_list.size();
		Data::Type<T>::setClassId(batchIndex);

		// Add new Batch
		IDataBatch* dataBatch = new DataBatch<T>();
		m_dataBatch_list.push_back(dataBatch);

		return batchIndex;
	}

	template<typename T>
	void mapToData(Init_DataMapper* p_init)
	{
		int batchIndex = Data::Type<T>::classId();
		if(batchIndex == -1)
		{
			batchIndex = addBatch<T>();
		}

		DataBatch<T>* dataBatch = (DataBatch<T>*)m_dataBatch_list[batchIndex];
		dataBatch->mapToData<T>(p_init);
	}
};