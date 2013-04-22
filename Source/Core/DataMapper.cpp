#include "DataMapper.h"

#include "World.h"

IDataMapper::IDataMapper()
{
	m_data = WORLD()->manager_data();
}
