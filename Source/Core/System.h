#pragma once

#include "Data.h"
#include "ISystem.h"
#include "DataMapper.h"

namespace System
{
	class Translation : public Type<Translation>
	{
	private:
		DataMapper<Data::Transform> m_map_translation;

	public:
		void update()
		{
			static float i = 0.0f;
			DataMapper<Data::Transform> pos;
			while(m_map_translation.hasNext())
			{
				Data::Transform* trans = m_map_translation.next();
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