#pragma once

#include "Data.h"
#include "ISystem.h"
#include "DataMapper.h"

namespace System
{
	class Translation : public Type<Translation>
	{
	private:
		DataMapper<Data::Transform> m_mapper_position;

	public:
		void update()
		{
			while(m_mapper_position.hasNext())
			{
			//Data::Position* position = m_mapper_position.next();
			}
		}

		void process()
		{
		}

		std::string className()
		{
			return "Translation";
		}
	};
}