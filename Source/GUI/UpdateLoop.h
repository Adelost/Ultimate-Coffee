#pragma once

#include <Core/IObserver.h>

class World;
class UpdateTimer;

// Class 
class UpdateLoop : IObserver
{
private:
	World* m_world;
	UpdateTimer* m_updateTimer;

public:
	UpdateLoop();
	~UpdateLoop();

	void init();
	void onEvent(Event* e);
	void update();
	void computeFPS();

};