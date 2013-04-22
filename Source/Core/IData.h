#pragma once

#include "Util.h"

// Interface to all Data
class IData
{
public:
	//virtual int classId() = 0;
	//virtual void setClassId(int p_id) = 0;
};

namespace Data
{
	// Holds common data unique to the class
	template<typename T>
	class Type : public IData
	{
	private:
		// Unique id, used as a fast way
		// of indexing inside "manager"
		static int s_classId;	

	public:
		// Returns TRUE if the data 
		// has not yet received
		// an "id"
		bool isUnkown()
		{
			return s_classId == -1;
		}

		static int classId()
		{
			return s_classId;
		}

		static void setClassId(int p_id)
		{
			s_classId = p_id;
		}

		std::string className()
		{
			return "NOT_NAMED_YET";
		}
	};

	template<typename T>
	int Type<T>::s_classId = -1;
}

