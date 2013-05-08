//#pragma once
//
//#include <vector>
//
//template <class T>
//class BatchPointer
//{
//private:
//	static std::vector<T>* s_hostArray;
//	int m_index;
//
//public:
//	BatchPointer()
//	{
//		m_index = -1;
//	}
//
//	void init(int index)
//	{
//		m_index = index;
//	}
//
//	T* operator->()
//	{
//		std::vector<T>* v = s_hostArray;
//		return &v->at(m_index);
//	}
//
//	bool isValid()
//	{
//		return s_hostArray != -1;
//	}
//
//	void init(std::vector<T>* hostArray)
//	{
//		s_hostArray = hostArray;
//	}
//
//	static void initClass(std::vector<T>* hostArray)
//	{
//		s_hostArray = hostArray;
//		s_hostArray;
//	}
//};
//
//template<typename T>
//std::vector<T>* BatchPointer<T>::s_hostArray = nullptr;