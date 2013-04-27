#pragma once

#include <Core/World.h>
#include <System_Render/System_Render.h>
#include "Util.h"
#include "UpdateTimer.h"

// Class 
class UpdateLoop
{
private:
	World* m_world;
	UpdateTimer m_updateTimer;

public:
	UpdateLoop();

	void init();
	void update();
	void computeFPS();
};