#include "Window.h"

#include <Core/Events.h>
#include "UpdateLoop.h"

Window::Window()
{
	// Init window
	m_ui.setupUi(this);
	setDockOptions(AllowNestedDocks | AllowTabbedDocks);
	setIconSize(QSize(20, 20));
	setWindowTitle("Ultimate Coffee");
	m_manager_tools.init();
	m_manager_docks.init();
	m_manager_console.init();
	m_manager_commands.init();

	// Init architecture
	Math::init();
	SUBSCRIBE_TO_EVENT(this, EVENT_SHOW_MESSAGEBOX);

	// Start update loop
	m_updateLoop = new UpdateLoop();
	m_updateLoop->init();
	m_refreshTimer.setInterval(0);
	connect(&m_refreshTimer, SIGNAL(timeout()), this, SLOT(update()));
	m_refreshTimer.start();
}

Window::~Window()
{
	delete m_updateLoop;
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
	return &m_ui;
}

QIcon Window::createIcon( QColor p_color )
{
	QPixmap pixmap(16, 16);
	pixmap.fill(p_color);
	return QIcon(pixmap);
}
