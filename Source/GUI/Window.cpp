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

Window::Window()
{
	// Init window
	m_ui = new Ui::MainWindow();
	m_ui->setupUi(this);
	setDockOptions(AllowNestedDocks | AllowTabbedDocks);
	setIconSize(QSize(20, 20));
	setWindowTitle("Ultimate Coffee");
	//setWindowFlags( Qt::FramelessWindowHint );

	m_renderWidget = new RenderWidget(this);
	m_renderWidget->setMinimumSize(1, 1);

	m_manager_tools = new Manager_Tools();
	m_manager_tools->init();

	m_manager_docks = new Manager_Docks();
	m_manager_docks->init();

	m_manager_console = new Manager_Console();
	m_manager_console->init();

	m_manager_commands = new Manager_Commands();
	m_manager_commands->init();

	m_manager_docks->resetLayout();
	//addToAutoDelete(new SplashScreen(this));

	m_refreshTimer = new QTimer();

	// Init architecture
	Math::init();
	SUBSCRIBE_TO_EVENT(this, EVENT_SHOW_MESSAGEBOX);

	// Start update loop
	m_updateLoop = new UpdateLoop();
	m_updateLoop->init();
	m_refreshTimer->setInterval(0);
	connect(m_refreshTimer, SIGNAL(timeout()), this, SLOT(update()));
	m_refreshTimer->start();
}

Window::~Window()
{
	for(int i=0; i<(int)m_autoDelete.size(); i++)
		delete m_autoDelete[i];

	delete m_updateLoop;
	delete m_manager_commands;
	delete m_manager_console;
	delete m_manager_docks;
	delete m_manager_tools;
	delete m_refreshTimer;
	delete m_ui;
}

void Window::update()
{
	m_updateLoop->update();
}																			

void Window::setFullscreen( bool checked )
{
	if(checked)
		this->showFullScreen();
	else
		this->showNormal();
}

void Window::onEvent( IEvent* p_event )
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

Window* Window::instance()
{
	static Window instance;
	return &instance;
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

void Window::keyPressEvent( QKeyEvent *e )
{
	// Update camera
	Entity* entity_camera = CAMERA_ENTITY().asEntity();
	Data::Transform* d_transform = entity_camera->fetchData<Data::Transform>();
	Data::Camera* d_camera = entity_camera->fetchData<Data::Camera>();

	float delta = SETTINGS()->deltaTime * 1000.0f;
	float strafe = 0.0f;
	float walk = 0.0f;

	if(e->key() == Qt::Key_W)
	{
		walk += delta;
	}
	if(e->key() == Qt::Key_A)
	{
		strafe -= delta;
	}
	if(e->key() == Qt::Key_S)
	{
		walk -= delta;
	}
	if(e->key() == Qt::Key_D)
	{
		strafe += delta;
	}

	// Rotate camera
	d_camera->strafe(d_transform->position, strafe);
	d_camera->walk(d_transform->position, walk);
	d_camera->updateViewMatrix(d_transform->position);
}
