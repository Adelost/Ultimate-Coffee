#include "stdafx.h"
#include "Manager_Tools.h"
#include <Core/Enums.h>

#include <Core/World.h>
#include <Core/Enums.h>
#include <Core/Factory_Entity.h>
#include <Core/Events.h>
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
	connect(a, SIGNAL(triggered()), this, SLOT(newLevel()));

	// Exit
	a = m_ui->actionQuit;
	a->setShortcut(QKeySequence("Ctrl+Q"));
	connect(a, SIGNAL(triggered()), m_window, SLOT(close()));

	// HELP					
	// About
	a = new QAction("&About", this);
	connect(a, SIGNAL(triggered()), this, SLOT(action_about()));
	m_ui->menuHelp->addAction(a);
	// About Qt
	a = new QAction(tr("About &Qt"), this);
	connect(a, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	m_ui->menuHelp->addAction(a);
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
	createToolAction(mapper, Enum::Tool_Translate,	"Translate")->activate(QAction::Trigger);
	createToolAction(mapper, Enum::Tool_Rotate,		"Rotate");
	createToolAction(mapper, Enum::Tool_Scale,		"Scale");
	createToolAction(mapper, Enum::Tool_Geometry,	"Geometry");
	//createToolAction(mapper, Enum::Tool_Entity,		"Entity");

	// Context bar
	m_ui->contextBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
//	QMenu* menu;
// 	menu = new QMenu("Info", m_window);
// 	menu->setIcon(createIcon("Selection"));
// 	m_ui->contextBar->addAction(menu->menuAction());

	//m_ui->contextBar->addSeparator();
	a = createContextIcon("Coffee");
	connect(a, SIGNAL(triggered()), this, SLOT(coffee()));
	a = createContextIcon("Experiment");
	connect(a, SIGNAL(triggered()), this, SLOT(createAsteroids()));
	createContextIcon("Tool");

}

void Manager_Tools::action_about()
{
	QMessageBox::about(m_window, "About Ultimate Coffee",
		"Coffee... is a feeling.\n\nMattias Andersson\nNicolas Dubray\nNils Forsman\nHenrik Nell\nViktor Sid�n");
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

QAction* Manager_Tools::createToolAction( QSignalMapper* p_mapper, int p_type, std::string p_icon )
{
	std::string path = "";
	path = path + ICON_PATH + "Tools/" + p_icon;

	QAction* a = new QAction(QIcon(path.c_str()), p_icon.c_str(), m_window);
	a->setCheckable(true);
	m_toolGroup->addAction(a);
	m_ui->toolBar->addAction(a);

	connect(a, SIGNAL(triggered()), p_mapper, SLOT(map()));
	p_mapper->setMapping(a, p_type);

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
	SEND_EVENT(&Event_StoreCommandsAsSingleEntryInCommandHistoryGUI(&command_list, false));
}

void Manager_Tools::newLevel()
{
	SEND_EVENT(&Event(EVENT_NEW_LEVEL));
}
