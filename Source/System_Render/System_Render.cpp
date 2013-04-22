#include "System_Render.h"

void System::Render::update()
{
	while(m_mapper_position.hasNext())
	{
		Data::Position* position = m_mapper_position.next();
	}
}

