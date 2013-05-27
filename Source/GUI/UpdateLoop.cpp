#include "stdafx.h"
#include "UpdateLoop.h"
#include "UpdateTimer.h"
#include "Window.h"

#include <Core/System.h>
#include <Core/System_Input.h>
#include <Core/Manager_Entity.h>
#include <System_Render/System_Render.h>

UpdateLoop::UpdateLoop()
{
	m_updateTimer = new UpdateTimer();
	m_updateTimer->reset();
	m_world = WORLD();

	SUBSCRIBE_TO_EVENT(this, EVENT_NEW_PROJECT);
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

	// Init game
	SEND_EVENT(&Event(EVENT_NEW_PROJECT));

	// Create Entities
	SETTINGS()->entity_camera = FACTORY_ENTITY()->createEntity(Enum::Entity_Camera)->toPointer();
	FACTORY_ENTITY()->createEntity(Enum::Entity_Sky);

	FACTORY_ENTITY()->createEntity(Enum::Entity_DirLight);
	for(int i=0; i<1; i++)
	{
		FACTORY_ENTITY()->createEntity(Enum::Entity_Pointlight, true);
	}

	FACTORY_ENTITY()->createEntity(Enum::Entity_Mesh, true)->fetchData<Data::Render>()->setMesh(Enum::Mesh_Box);
	FACTORY_ENTITY()->createEntity(Enum::Entity_Mesh, true)->fetchData<Data::Render>()->setMesh(Enum::Mesh_Sphere);
	FACTORY_ENTITY()->createEntity(Enum::Entity_Mesh, true)->fetchData<Data::Render>()->setMesh(Enum::Mesh_Cylinder);
	FACTORY_ENTITY()->createEntity(Enum::Entity_Mesh, true)->fetchData<Data::Render>()->setMesh(Enum::Mesh_Cone);
	FACTORY_ENTITY()->createEntity(Enum::Entity_Mesh, true)->fetchData<Data::Render>()->setMesh(Enum::Mesh_Pyramid);
}

void UpdateLoop::update()
{
	// Update game
	computeFPS();
	m_updateTimer->tick();
	SETTINGS()->trueDeltaTime = m_updateTimer->deltaTime();
	if(SETTINGS()->runSimulation())
		SETTINGS()->deltaTime = SETTINGS()->trueDeltaTime;
	else
		SETTINGS()->deltaTime = 0.0f;
	m_world->update();

	// Update EventManager to enable queued messages
	EventManager::getInstance()->update(SETTINGS()->deltaTime);
}

void UpdateLoop::computeFPS()
{
	// NOTE: Need to clean up this code later on (vagt),
	// but it works for now (nowadays?).

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

void UpdateLoop::onEvent( Event* e )
{
	EventType type = e->type();
	switch (type)
	{
	case EVENT_NEW_PROJECT: // Also refer to Manager_Commands::onEvent
		{
			// Remove previous
			//WORLD()->manager_entity()->clear();

			
		}
		break;
	default:
		break;
	}
}
