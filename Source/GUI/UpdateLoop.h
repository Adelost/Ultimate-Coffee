#pragma once

class World;
class UpdateTimer;

// Class 
class UpdateLoop
{
private:
	World* m_world;
	UpdateTimer* m_updateTimer;

public:
	UpdateLoop();
	~UpdateLoop();

	void init();
	void update();
	void computeFPS();
};