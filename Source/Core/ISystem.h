#pragma once

// Interface to all systems
class ISystem
{
public:

public:
	virtual ~ISystem(){}
	virtual void update() = 0;
	virtual int classId() = 0;
	virtual void setClassId(int p_id) = 0;
	virtual std::string className() = 0;
};

namespace System
{
	// Holds common data unique to the class
	template<typename T>
	class Type : public ISystem
	{
	public:
		// Unique id, used as a fast way
		// of indexing inside "manager"
		static int s_classId;

	public:
		int classId()
		{
			return s_classId;
		}

		void setClassId(int p_id)
		{
			s_classId = p_id;
		}

		virtual std::string className()
		{
			return "NOT_NAMED_YET";
		}
	};

	template<typename T>
	int Type<T>::s_classId = -1;
}