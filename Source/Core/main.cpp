#include "Util.h"
#include "World.h"


int main()
{
	// Detect memory leaks
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	World* world = WORLD();

	// Init systems
	world->addSystem(new System::Translation);
	
	// Create Entities
	Entity e(0);
	e.fetchData<Data::Position>();
	e.removeData<Data::Position>();
	e.addData(Data::Position());

	world->update();

	return 0;
}