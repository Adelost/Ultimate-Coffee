#include "stdafx.h"
#include "Manager_Systems.h"

Manager_Systems::Manager_Systems()
{
	m_systems = new std::vector<ISystem*>();
}

Manager_Systems::~Manager_Systems()
{
	for(int i=0; i<(int)m_systems->size(); i++)
		delete m_systems->at(i);

	delete m_systems;
}

void Manager_Systems::addSystem( ISystem* p_system)
{
	int systemIndex = m_systems->size();
	p_system->setClassId(systemIndex); 

	m_systems->push_back(p_system);
}

void Manager_Systems::update()
{
	for(int i=0; i<(int)m_systems->size(); i++)
	{
		m_systems->at(i)->update();
	}
}

