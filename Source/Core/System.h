#pragma once

#include "ISystem.h"
#include "DataMapper.h"

namespace System
{
	class Translation : public Type<Translation>
	{
	private:
		DataMapper<Data::Position> m_mapper_position;
		std::vector<int>* m_owner_list;

	public:
		void update()
		{
			/*while(m_mapper_position.hasNext())
			{
			Data::Position* position = m_mapper_position.next();
			}*/
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