#include "stdafx.h"
#include "UpdateLoop.h"
#include "UpdateTimer.h"
#include "Window.h"

#include <Core/System.h>
#include <Core/System_Input.h>
#include <System_Render/System_Render.h>

UpdateLoop::UpdateLoop()
{
	m_updateTimer = new UpdateTimer();
	m_updateTimer->reset();
	m_world = WORLD();
}

UpdateLoop::~UpdateLoop()
{
	delete m_updateTimer;
}

void UpdateLoop::init()
{
	// Init systems
	m_world->addSystem(new System::Translation());
	m_world->addSystem(new System::Input());
	m_world->addSystem(new System::Render());
	m_world->addSystem(Window::instance()->system_editor());
	m_world->addSystem(new System::Test());
	
	// Create Entities

	SETTINGS()->entity_camera = FACTORY_ENTITY()->createEntity(ENTITY_CAMERA)->toPointer();
	FACTORY_ENTITY()->createEntity(ENTITY_SKY);

	FACTORY_ENTITY()->createEntity(ENTITY_POINTLIGHT);
	 
	for(int i=0; i<1000; i++)
	{
		FACTORY_ENTITY()->createEntity(ENTITY_CUBE);
	}
}

void UpdateLoop::update()
{
	// Update game
	computeFPS();
	m_updateTimer->tick();
	SETTINGS()->deltaTime = m_updateTimer->deltaTime();
	m_world->update();

	// Update EventManager to enable queued messages
	EventManager::getInstance()->update(SETTINGS()->deltaTime);
}

void UpdateLoop::computeFPS()
{
	// NOTE: Needs to clean up this code later on,
	// but it works for now.

	static int num_frames = 0;
	static float timeElapsed = 0.0f;
	num_frames++;

	// Compute averages FPS and ms over one second period.
	if((m_updateTimer->totalTime() - timeElapsed) >= 1.0f)
	{
		// calculate statistics
		float fps = (float)num_frames; // fps = frameCnt / 1
		float msPerFrame = 1000.0f/fps;

		// convert statistics into QString
		QString stats;
		stats = "FPS:  %1  Frame Time:  %2 (ms)";
		stats = stats.arg(fps).arg((int)msPerFrame);

		// send signal

		//static int debugId = StopWatch::getUniqueId();
		//SEND_EVENT(&Event_PostDebugMessage(debugId, "Total: " + Converter::IntToStr(fps) + " (fps)"));

		//statusBar()->showMessage(stats);

		Window::instance()->setWindowTitle("Ultimate Coffee - " + stats);

		// reset stats for next average.
		num_frames = 0;
		timeElapsed += 1.0f;
	}
}
