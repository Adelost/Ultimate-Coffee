#include "stdafx.h"
#include "Manager_Tools.h"

#include <Core/World.h>
#include <Core/Events.h>
#include "Window.h"
#include "ui_MainWindow.h"

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
	a->setStatusTip(tr("NO FUNCTIONALITY YET (2013-04-24, 14.51)"));

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
	createToolAction(mapper, SELECTION, "Selection")->setChecked(true);
	createToolAction(mapper, TRANSLATE, "Translate");
	createToolAction(mapper, ROTATE,	"Rotate");
	createToolAction(mapper, SCALE,		"Scale");
	createToolAction(mapper, GEOMETRY,	"Geometry");
	createToolAction(mapper, ENTITY,	"Entity");

	// Context bar
	m_ui->contextBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
	QMenu* menu;
	menu = new QMenu("Info", m_window);
	menu->setIcon(createIcon("Selection"));
	m_ui->contextBar->addAction(menu->menuAction());

	m_ui->contextBar->addSeparator();
	createContextIcon("Toast");
	createContextIcon("Coffee");
	createContextIcon("Wine");
	createContextIcon("Experiment");
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

QAction* Manager_Tools::createToolAction( QSignalMapper* p_mapper, ToolType p_type, std::string p_icon )
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
	m_selectedTool = static_cast<ToolType>(p_toolType);

	bool unset = false;

	switch(m_selectedTool)
	{
	case TRANSLATE:
		{
				bool isUsed = SETTINGS()->leftMousePressed;
				if(isUsed)
					m_window->renderWidget()->setCursor(Qt::SizeHorCursor);
				else
					m_window->renderWidget()->setCursor(Qt::SizeAllCursor);	
		}
		break;
	case ROTATE:
		{
			bool isUsed = SETTINGS()->leftMousePressed;
			if(isUsed)
				m_window->renderWidget()->setCursor(Qt::ClosedHandCursor);
			else
				m_window->renderWidget()->setCursor(Qt::OpenHandCursor);	
		}
		break;
	case GEOMETRY:
		{
			m_window->renderWidget()->setCursor(Qt::CrossCursor);
		}
		break;
	case SCALE:
		{
			m_window->renderWidget()->setCursor(Qt::PointingHandCursor);
		}
		break;
	case ENTITY:
		{
			m_window->renderWidget()->setCursor(Qt::ForbiddenCursor);
		}
		break;
	default:
		{
			unset = true;
			break;
		}		
	}

	if(unset)
		m_window->renderWidget()->unsetCursor();
}

void Manager_Tools::onEvent( IEvent* p_event )
{
	EventType type = p_event->type();
	switch (type) 
	{
	case EVENT_SET_TOOL:
		{
			setTool(m_selectedTool);
		}
		break;
	default:
		break;
	}
}

