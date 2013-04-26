#pragma once

#include <Core/World.h>
#include <System_Render/System_Render.h>

// Class 
class UpdateLoop
{
private:
	World* m_world;

public:
	UpdateLoop()
	{
		m_world = WORLD();
	}

	void init()
	{
		// Init systems
		m_world->addSystem(new System::Translation());
		m_world->addSystem(new System::Render());

		// Create Entities
		Entity* e = m_world->createEntity();
		e->fetchData<Data::Position>();
		e->removeData<Data::Position>();
		e->addData(Data::Position());
	}

	void update()
	{
		// Update game
		m_world->update();
	}
};