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
	new SplashScreen(this);

	m_refreshTimer = new QTimer(this);

	// Init architecture
	Math::init();
	SUBSCRIBE_TO_EVENT(this, EVENT_SHOW_MESSAGEBOX);

	// Start update loop
	m_updateLoop = new UpdateLoop();
	m_updateLoop->init();
	m_refreshTimer->setInterval(0);
	connect(m_refreshTimer, SIGNAL(timeout()), this, SLOT(update()));
	m_refreshTimer->start();


	SEND_EVENT(&Event_SetCursor(Event_SetCursor::CustomCursor));
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
	return new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
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

Window* Window::s_instance = nullptr;

SplashScreen::SplashScreen( Window* parent ) : QDockWidget("Welcome to Ultimate Coffee", parent)
{
	parent->addDockWidget(Qt::RightDockWidgetArea, this);
	setFloating(true);
	setAllowedAreas(Qt::NoDockWidgetArea);
	setFixedSize(180, 100);
	move(parent->x() + parent->width()/2 - width()/2, parent->y() +  + parent->height()/2 - height()/2);

	// Create buttons
	QWidget* w = new QWidget();
	setWidget(w);
	QLayout* l = new QVBoxLayout();
	w->setLayout(l);
	QPushButton* b;
	b = new QPushButton("New");
	connect(b, SIGNAL(clicked()), this, SLOT(newFile()));
	l->addWidget(b);
	b = new QPushButton("Load");
	connect(b, SIGNAL(clicked()), this, SLOT(openFile()));
	l->addWidget(b);
	b = new QPushButton("Recent");
	connect(b, SIGNAL(clicked()), this, SLOT(recentFile()));
	l->addWidget(b);
	

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

void SplashScreen::newFile()
{
	MESSAGEBOX("'newFile' is not implemented yet.");
	close();
}

void SplashScreen::openFile()
{
	Window::instance()->ui()->actionOpen->trigger();
	close();
}

void SplashScreen::recentFile()
{
	Window::instance()->ui()->actionRecent->trigger();
	close();
}
