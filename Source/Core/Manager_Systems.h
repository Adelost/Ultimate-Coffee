#pragma once

#include "ISystem.h"

class Manager_Systems
{
private:
	std::vector<ISystem*> m_systems;

public:
	Manager_Systems()
	{
	}

	~Manager_Systems()
	{
		for(int i=0; i<(int)m_systems.size(); i++)
			delete m_systems[i];
	}
	// Adds system to manager and saves the index
	// inside the System to make it easier to identify
	// and lookup later
	void addSystem(ISystem* p_system)
	{
		int systemIndex = m_systems.size();
		p_system->setClassId(systemIndex); 

		m_systems.push_back(p_system);
	}

	// Update systems in the order they was added
	void update()
	{
		for(int i=0; i<(int)m_systems.size(); i++)
		{
			m_systems[i]->update();
		}
	}
};