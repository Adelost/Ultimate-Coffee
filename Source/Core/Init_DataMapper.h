#pragma once

#include <vector>

class Init_DataMapper
{
private:
	void* m_data_list;

public:
	std::vector<int>* owner_list;
	int index_lastGap;

public:
	void setDataList(void* p_data_list)
	{
		m_data_list = p_data_list;
	}
	template<typename T>
	std::vector<T>* dataList()
	{
		return static_cast<std::vector<T>*>(m_data_list);
	}
};