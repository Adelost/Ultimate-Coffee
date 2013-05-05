#pragma once

class ISystem;
#include <vector>

class Manager_Systems
{
private:
	std::vector<ISystem*>* m_systems;

public:
	Manager_Systems();
	~Manager_Systems();

	// Adds system to manager and saves the index
	// inside the System to make it easier to identify
	// and lookup later
	void addSystem(ISystem* p_system);

	// Update systems in the order they was added
	void update();
};