#include "stdafx.h"
#include "Window.h"
#include "UpdateLoop.h"
#include "Manager_Commands.h"
#include "Manager_Docks.h"
#include "Manager_Tools.h"
#include "Manager_Console.h"
#include "RenderWidget.h"
#include "ui_MainWindow.h"

#include <Core/World.h>
#include <Core/Data.h>
#include <Core/Entity.h>
#include <Core/Events.h>


void Window::init()
{
	SUBSCRIBE_TO_EVENT(this, EVENT_SHOW_MESSAGEBOX);

	// Init window
	m_ui = new Ui::MainWindow();
	m_ui->setupUi(this);
	setDockOptions(AllowNestedDocks | AllowTabbedDocks);
	setContextMenuPolicy(Qt::PreventContextMenu);
	setIconSize(QSize(20, 20));
	setWindowTitle("Ultimate Coffee");

	m_manager_console = new Manager_Console();
	m_manager_console->init();

	m_renderWidget = new RenderWidget(this);
	m_renderWidget->setMinimumSize(1, 1);

	m_manager_tools = new Manager_Tools();
	m_manager_tools->init();

	m_manager_docks = new Manager_Docks();
	m_manager_docks->init();

	m_manager_commands = new Manager_Commands();
	m_manager_commands->init();

	m_manager_docks->resetLayout();
	m_ui->menuWindow->addAction((new SplashScreen(this))->toggleViewAction());

	m_refreshTimer = new QTimer(this);

	// Init architecture
	Math::init();

	// Start update loop
	m_updateLoop = new UpdateLoop();
	m_updateLoop->init();
	m_refreshTimer->setInterval(0);
	connect(m_refreshTimer, SIGNAL(timeout()), this, SLOT(update()));
	m_refreshTimer->start();


	SEND_EVENT(&Event_SetCursor(Event_SetCursor::SceneCursor));
}

Window::~Window()
{
	delete m_updateLoop;
	delete m_manager_commands;
	delete m_manager_console;
	delete m_manager_docks;
	delete m_manager_tools;
	delete m_ui;
}

void Window::update()
{
	m_updateLoop->update();

	// Make sure all events is processed.
	// This is to prevent the QTimer from 
	// freezing the application in newer 
	// versions of Qt.
	//qApp->processEvents(QEventLoop::AllEvents);
	m_refreshTimer->setInterval(2);
}																			

void Window::setFullscreen( bool checked )
{
	if(checked)
		this->showFullScreen();
	else
		this->showNormal();
}

void Window::onEvent( Event* p_event )
{
	EventType type = p_event->type();
	switch (type)
	{
	case EVENT_SHOW_MESSAGEBOX:
		{
			QString message(static_cast<Event_ShowMessageBox*>(p_event)->message.c_str());
			QMessageBox::information(0, " ", message);
		}
		break;
	default:
		break;
	}
}

Ui::MainWindow* Window::ui()
{
	return m_ui;
}

QIcon Window::createIcon( QColor* p_color )
{
	QPixmap pixmap(16, 16);
	pixmap.fill(*p_color);
	return QIcon(pixmap);
}

QWidget* Window::renderWidget()
{
	return m_renderWidget;
}

QSpacerItem* Window::createSpacer( Qt::Orientation p_orientation )
{
	QSpacerItem* spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

	return spacer;
}

ISystem* Window::system_editor()
{
	return m_manager_docks->getAsSystem();
}

bool Window::eventFilter( QObject* object, QEvent* event )
{
	/*QEvent::Type type = event->type();

	if(type == QEvent::Timer)
	{
	DEBUGPRINT("Event: Timer");
	this::
	return false;
	}*/

	return false;
}

void Window::setRefreshInterval( int p_interval )
{
	m_refreshTimer->setInterval(p_interval);
}

Window* Window::instance()
{
	if(!s_instance)
		s_instance = new Window();

	return s_instance;
}

void Window::keyPressEvent( QKeyEvent *e )
{
	QCoreApplication::sendEvent(m_renderWidget, e);
}

void Window::keyReleaseEvent( QKeyEvent *e )
{
	QCoreApplication::sendEvent(m_renderWidget, e);
}

Window* Window::s_instance = nullptr;

SplashScreen::SplashScreen( Window* parent ) : QDockWidget("Welcome to Ultimate Coffee", parent)
{
	parent->addDockWidget(Qt::RightDockWidgetArea, this);
	setFloating(true);
	setAllowedAreas(Qt::NoDockWidgetArea);
	setFixedSize(180, 150);
	move(parent->x() + parent->width()/2 - width()/2, parent->y() +  + parent->height()/2 - height()/2);

	// Create buttons
	QWidget* w = new QWidget();
	setWidget(w);
	QLayout* l = new QVBoxLayout();
	w->setLayout(l);
	QPushButton* b;
	b = new QPushButton("New");
	connect(b, SIGNAL(clicked()), this, SLOT(newProject()));
	l->addWidget(b);
	b = new QPushButton("Load");
	connect(b, SIGNAL(clicked()), this, SLOT(loadProject()));
	l->addWidget(b);
	b = new QPushButton("Recent");
	connect(b, SIGNAL(clicked()), this, SLOT(loadRecentProject()));
	l->addWidget(b);
	{
		//QHBoxLayout* hl = new QHBoxLayout();
		//QLayout* hl = new QHBoxLayout();
		//l->addWidget(new QLabel("     "));
		//hl->addWidget(new QCheckBox("Show at Startup2"));
		//hl->addItem(new QSpacerItem(2000, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
		QComboBox* combo = new QComboBox();
		connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeColorScheme(int)));
		combo->addItem("RGB mode");
		combo->addItem("CMY mode");
		l->addWidget(combo);
		QCheckBox* cb = new QCheckBox("Show at Startup");
		cb->setChecked(true);
		l->addWidget(cb);
	}

	

	/*
		QLabel* l = new QLabel("");
		std::string path = "";

		resize(sizeHint());
		move(parent->x() + parent->x()/2 + width(), parent->y() + parent->y()/2 - height()/2);
		path = path + ICON_PATH + "Misc/" + "spash";
		l->setPixmap(QPixmap(path.c_str()));
		*/
}

SplashScreen::~SplashScreen()
{
	int i = 0;
	i = 0;
}

void SplashScreen::newProject()
{
	SEND_EVENT(&Event(EVENT_NEW_PROJECT));
	close();
}

void SplashScreen::loadProject()
{
	Window::instance()->ui()->actionOpen->trigger();
	close();
}

void SplashScreen::loadRecentProject()
{
	Window::instance()->ui()->actionRecent->trigger();
	close();
}

void SplashScreen::changeColorScheme( int enum_scheme )
{
	Enum::ColorScheme scheme = static_cast<Enum::ColorScheme>(enum_scheme);

	SETTINGS()->m_ColorScheme_3DManipulatorWidgets = scheme;
}
