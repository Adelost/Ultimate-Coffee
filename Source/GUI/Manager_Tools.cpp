#include "stdafx.h"
#include "Manager_Tools.h"

#include <Core/Enums.h>
#include <Core/World.h>
#include <Core/Enums.h>
#include <Core/Factory_Entity.h>
#include <Core/Events.h>
#include <Core/DataMapper.h>
#include "Window.h"
#include "ui_MainWindow.h"
#include <Core/Command_CreateEntity.h>

void Manager_Tools::init()
{
	m_window = Window::instance();
	m_ui = m_window->ui();
	setupActions();
	setupToolbar();

	SUBSCRIBE_TO_EVENT(this, EVENT_SET_TOOL);
}

void Manager_Tools::setupToolbar()
{
	// Actions
	QAction* a;
	std::string iconPath = ICON_PATH;
	std::string path;

	// FILE
	// New
	a = m_ui->actionNew;
	//path = iconPath + "Menu/new";
	//a->setIcon(QIcon(path.c_str())); 
	a->setShortcuts(QKeySequence::New);
	connect(a, SIGNAL(triggered()), this, SLOT(newProject()));

	// Exit
	a = m_ui->actionQuit;
	a->setShortcut(QKeySequence("Ctrl+Q"));
	connect(a, SIGNAL(triggered()), m_window, SLOT(close()));

	// HELP					
	// About
	a = new QAction("About", this);
	connect(a, SIGNAL(triggered()), this, SLOT(action_about()));
	m_ui->menuHelp->addAction(a);
	// About Qt
	a = new QAction(tr("About &Qt"), this);
	connect(a, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	m_ui->menuHelp->addAction(a);

	// EDIT					
	a = m_ui->actionCopy;
	connect(a, SIGNAL(triggered()), this, SLOT(action_copy()));
	a->setShortcut(QKeySequence("Ctrl+C"));
	a = m_ui->actionPaste;
	connect(a, SIGNAL(triggered()), this, SLOT(action_paste()));
	a->setShortcut(QKeySequence("Ctrl+V"));
}

void Manager_Tools::setupActions()
{
	QAction* a;
	QSignalMapper* mapper = new QSignalMapper(m_window);
	connect(mapper, SIGNAL(mapped(int)), this, SLOT(setTool(int)));

	// Toolbar
	m_toolGroup = new QActionGroup(this);
	m_ui->toolBar->setIconSize(QSize(18,18));
	//createToolAction(mapper, Enum::Tool_Selection, "Selection");
	createToolAction(mapper, Enum::Tool_Translate,	"Translate", "Select and translate (1)")->activate(QAction::Trigger);
	createToolAction(mapper, Enum::Tool_Rotate,		"Rotate", "Select and rotate (2)");
	createToolAction(mapper, Enum::Tool_Scale,		"Scale", "Select and scale (3)");
	createToolAction(mapper, Enum::Tool_Geometry,	"Geometry", "Create entity (4)");
	//createToolAction(mapper, Enum::Tool_Entity,		"Entity");

	// Context bar
	m_ui->contextBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
//	QMenu* menu;
// 	menu = new QMenu("Info", m_window);
// 	menu->setIcon(createIcon("Selection"));
// 	m_ui->contextBar->addAction(menu->menuAction());

	//m_ui->contextBar->addSeparator();
// 	a = createContextIcon("Coffee");
// 	a->setToolTip("Recreate geometry");
// 	connect(a, SIGNAL(triggered()), this, SLOT(coffee()));

	a = createContextIcon("preview");
	a->setToolTip("Preview item browser");
	connect(a, SIGNAL(triggered(bool)), this, SLOT(previewItemBrowser()));

	a = createContextIcon("image");
	a->setToolTip("Import image");
	connect(a, SIGNAL(triggered(bool)), this, SLOT(loadImage()));

	a = createContextIcon("asteroid");
	a->setToolTip("Create 1 asteroid");
	connect(a, SIGNAL(triggered()), this, SLOT(createAsteroid()));

	a = createContextIcon("asteroids");
	a->setToolTip("Create 1000 asteroids");
	connect(a, SIGNAL(triggered()), this, SLOT(createAsteroids()));

	a = createContextIcon("homing");
	a->setToolTip("Homing asteroids");
	a->setCheckable(true);
	connect(a, SIGNAL(triggered(bool)), this, SLOT(homingAsteroids(bool)));

	a = createContextIcon("simulate");
	a->setToolTip("Run simulation");
	a->setCheckable(true);
	a->setChecked(true);
	connect(a, SIGNAL(triggered(bool)), this, SLOT(runSimulation(bool)));

	a = createContextIcon("culling");
	a->setToolTip("Do frustum culling");
	a->setCheckable(true);
	a->setChecked(true);
	connect(a, SIGNAL(triggered(bool)), this, SLOT(doFrustumCulling(bool)));

	a = createContextIcon("oculus");
	a->setToolTip("Enable Oculus");
	a->setToolTip("");
	a->setCheckable(true);
	a->setChecked(true);
	connect(a, SIGNAL(triggered(bool)), this, SLOT(toggleOcculus(bool)));
}

void Manager_Tools::action_about()
{
	QMessageBox::about(m_window, "About Ultimate Coffee",
		"Coffee... is a feeling.\n\nMattias Andersson\nNicolas Dubray\nHenrik Nell\nViktor Sidén");
}

QAction* Manager_Tools::createContextIcon( std::string p_icon )
{
	std::string path = "";
	path = path + ICON_PATH + "Tools/" + p_icon;
	
	QAction* a = new QAction(QIcon(path.c_str()), p_icon.c_str(), m_window);
	m_ui->contextBar->addAction(a);

	return a;
}

QIcon Manager_Tools::createIcon( std::string p_icon )
{
	std::string path = "";
	path = path + ICON_PATH + "Tools/" + p_icon;

	return QIcon(path.c_str());
}

QAction* Manager_Tools::createToolAction( QSignalMapper* mapper, int type, std::string icon, std::string toolTip )
{
	std::string path = "";
	path = path + ICON_PATH + "Tools/" + icon;

	QAction* a = new QAction(QIcon(path.c_str()), icon.c_str(), m_window);
	a->setToolTip(toolTip.c_str());
	a->setCheckable(true);
	m_toolGroup->addAction(a);
	m_ui->toolBar->addAction(a);

	connect(a, SIGNAL(triggered()), mapper, SLOT(map()));
	mapper->setMapping(a, type);

	return a;
}

void Manager_Tools::setTool( int p_toolType )
{
	SETTINGS()->setSelectedTool(p_toolType);
}

void Manager_Tools::onEvent( Event* p_event )
{
	EventType type = p_event->type();
	switch(type)
	{
	case EVENT_SET_TOOL:
		{
			QList<QAction*> list = m_toolGroup->actions();
			// HACK: Our index doesn't match with QActions
			// correct manually in the meantime.
			int toolIndex = SETTINGS()->selectedTool() - 2;
			if(toolIndex < list.count())
				list[toolIndex]->setChecked(true);
		}
		break;
	default:
		break;
	}
}

void Manager_Tools::coffee()
{
	SEND_EVENT(&Event(EVENT_COFFEE));
}

void Manager_Tools::createAsteroids()
{
	std::vector<Command*> command_list;
	for(int i=0; i<1000; i++)
	{
		// Create Entity
		Entity* e = FACTORY_ENTITY()->createEntity(Enum::Entity_Asteroid);
		Data::Transform* d_transform = e->fetchData<Data::Transform>();

		// Create command
		command_list.push_back(new Command_CreateEntity(e));
	}
	// Add to history

	//check. If an entity has a name that needs to be saved to file, put it in the data struct of the command (Henrik, 2013-05-18, 14.34)
	//command_list.back()->setName("New asteroid");
	SEND_EVENT(&Event_AddToCommandHistory(&command_list, false));
}

void Manager_Tools::newProject()
{
	SEND_EVENT(&Event(EVENT_NEW_PROJECT));
}

void Manager_Tools::createAsteroid()
{
	std::vector<Command*> command_list;
	for(int i=0; i<1; i++)
	{
		// Create Entity
		Entity* e = FACTORY_ENTITY()->createEntity(Enum::Entity_Asteroid);
		Data::Transform* d_transform = e->fetchData<Data::Transform>();

		// Create command
		command_list.push_back(new Command_CreateEntity(e));
	}
	// Add to history

	//check. If an entity has a name that needs to be saved to file, put it in the data struct of the command (Henrik, 2013-05-18, 14.34)
	//command_list.back()->setName("New asteroid");
	SEND_EVENT(&Event_AddToCommandHistory(&command_list, false));
}

void Manager_Tools::runSimulation( bool state )
{
	SETTINGS()->setRunSimulation(state);
}

void Manager_Tools::action_copy()
{
	// Clear previous clipboard
	Data::AddedToClipboard::clearClipboard();

	// Add selected Entities to clipboard
	DataMapper<Data::Selected> map_selected;
	while(map_selected.hasNext())
	{
		Entity* e = map_selected.nextEntity();
		e->addData(Data::AddedToClipboard());
	}
}

void Manager_Tools::action_paste()
{
	std::vector<Command*> command_list;

	// Clear previous selection
	Data::Selected::clearSelection();

	// Clone entities
	DataMapper<Data::AddedToClipboard> map_clipboard;
	while(map_clipboard.hasNext())
	{
		Entity* e = map_clipboard.nextEntity();
		// HACK: Prevent copying of certain objects
		int type = e->type();
		if(type != Enum::Entity_Sky && type != Enum::Entity_DirLight && type != Enum::Entity_Empty)
		{
			// Make sure clone is not added to clipboard as well
			Entity* clone = e->clone();
			clone->removeData<Data::AddedToClipboard>();

			// HACK: Recover data from cloning
			Data::Render* d_render = clone->fetchData<Data::Render>();
			if(d_render)
			{
				d_render->recoverFromCloning(clone);
			}

			// Select new entity
			Data::Selected::select(clone);

			// Save command
			command_list.push_back(new Command_CreateEntity(clone, true));
		}
	}

	// Inform about selection
	SEND_EVENT(&Event(EVENT_ENTITY_SELECTION));

	// Add to history
	if(command_list.size()>0)
	{
		SEND_EVENT(&Event_AddToCommandHistory(&command_list, false));
	}
}

void Manager_Tools::homingAsteroids( bool state )
{
	Data::Movement_Floating::targetCamera = state;
}


void Manager_Tools::doFrustumCulling( bool state )
{
	Data::Bounding::s_doFrustumCulling = state;

	// HACK: Make all culled objects visible when turned off
	if(!state)
	{
		DataMapper<Data::Bounding> map_bounding;
		while(map_bounding.hasNext())
		{
			Entity* entity = map_bounding.nextEntity();
			Data::Bounding* d_bounding  = entity->fetchData<Data::Bounding>();
			d_bounding->insideFrustum = true;
		}
	}
}


void Manager_Tools::loadImage()
{
	std::vector<Command*> command_list;

	// Prepare camera
	Entity* cam = CAMERA_ENTITY().asEntity();
	Data::Transform* d_cam_transform = cam->fetchData<Data::Transform>();

	// Load pixmap
	QString path = ICON_PATH;
	if(SETTINGS()->button.key_shift)
		path += "Images/lisa";
	else if(SETTINGS()->button.key_alt)
		path += "Images/ski";
	else
		path += "Images/invader";
	QPixmap pixmap(path);
	QImage image(path);

	Vector3 offset = d_cam_transform->position;
	offset.x -= image.size().width()/2;
	offset.y -= image.size().height()/2;

	// Create image out of cubes
	for(int x=0; x<image.size().width(); x++)
	{
		for(int y=0; y<image.size().height(); y++)
		{
			QColor p(image.pixel(x, y));
			
			// Make white act as transparent by not creating
			// any entity
			if(!(p.red() == 255 && p.green() == 255 && p.blue() == 255))
			{
				Entity* e = WORLD()->factory_entity()->createEntity(Enum::Entity_Mesh);
				Data::Transform* d_transform = e->fetchData<Data::Transform>();
				Data::Render* d_render = e->fetchData<Data::Render>();
				Data::Movement_Floating* d_float = e->addData(Data::Movement_Floating());
				//d_float->rotation = Vector3();
				d_float->velocity = Vector3();

				//d_transform->position = d_cam_transform->position;

				d_transform->position.x = x;
				d_transform->position.y = image.size().height() - y;
				d_transform->position.z = 60.0f;
				d_transform->position += offset;

				Color color(p.redF(), p.greenF(), p.blueF());
				d_render->mesh.color = color;

				command_list.push_back(new Command_CreateEntity(e, true));
			}
		}
	}

	// Save to history
	if(command_list.size() > 0)
		SEND_EVENT(&Event_AddToCommandHistory(&command_list, false));
}

void Manager_Tools::previewItemBrowser()
{
	SEND_EVENT(&Event(EVENT_PREVIEW_ITEMS));
}

void Manager_Tools::toggleOcculus( bool state )
{
	SETTINGS()->bOculusRift = state;
	SEND_EVENT(&Event_WindowResize(SETTINGS()->windowSize.x, SETTINGS()->windowSize.y));
}
