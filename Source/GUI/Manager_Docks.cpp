#include "Manager_Docks.h"
#include "RenderWidget.h"
#include "Window.h"
#include "ui_MainWindow.h"
#include "Util.h"

Manager_Docks::~Manager_Docks()
{
	delete m_renderWidget;
}

void Manager_Docks::init()
{
	m_window = Window::instance();
	m_menu = m_window->ui()->menuWindow;
	m_renderWidget = new RenderWidget(m_window);
	setupMenu();
}

void Manager_Docks::saveLayout()
{
	QString fileName = QFileDialog::getSaveFileName(m_window, tr("Save layout"));
	if(fileName.isEmpty())
		return;
	QFile file(fileName);
	if (!file.open(QFile::WriteOnly))
	{
		QString msg = tr("Failed to open %1\n%2")
			.arg(fileName)
			.arg(file.errorString());
		QMessageBox::warning(m_window, tr("Error"), msg);
		return;
	}

	QByteArray geo_data = m_window->saveGeometry();
	QByteArray layout_data = m_window->saveState();

	bool ok = file.putChar((uchar)geo_data.size());
	if(ok)
		ok = file.write(geo_data) == geo_data.size();
	if(ok)
		ok = file.write(layout_data) == layout_data.size();
	if (!ok)
	{
		QString msg = tr("Error writing to %1\n%2")
			.arg(fileName)
			.arg(file.errorString());
		QMessageBox::warning(m_window, tr("Error"), msg);
		return;
	}
}

void Manager_Docks::loadLayout()
{
	QString fileName = QFileDialog::getOpenFileName(m_window, tr("Load layout"));
	if(fileName.isEmpty())
		return;
	QFile file(fileName);
	if (!file.open(QFile::ReadOnly))
	{
		QString msg = tr("Failed to open %1\n%2")
			.arg(fileName)
			.arg(file.errorString());
		QMessageBox::warning(m_window, tr("Error"), msg);
		return;
	}

	uchar geo_size;
	QByteArray geo_data;
	QByteArray layout_data;

	bool ok = file.getChar((char*)&geo_size);
	if(ok)
	{
		geo_data = file.read(geo_size);
		ok = geo_data.size() == geo_size;
	}
	if(ok)
	{
		layout_data = file.readAll();
		ok = layout_data.size() > 0;
	}

	if (ok)
		ok = m_window->restoreGeometry(geo_data);
	if (ok)
		ok = m_window->restoreState(layout_data);

	if (!ok)
	{
		QString msg = tr("Error reading %1")
			.arg(fileName);
		QMessageBox::warning(m_window, tr("Error"), msg);
		return;
	}
}

void Manager_Docks::setupMenu()
{
	// Add dock action
	QAction* a;
	a = new QAction("Create Dock", this);
	a->setShortcuts(QKeySequence::AddTab);
	connect(a, SIGNAL(triggered()), this, SLOT(createDockWidget()));
	m_menu->addAction(a);

	// Scene
	m_window->centralWidget()->hide();
	QDockWidget* dock;
	dock = new QDockWidget(tr("Scene"), m_window);
	m_window->addDockWidget(Qt::LeftDockWidgetArea, dock);
	a = dock->toggleViewAction();
	m_menu->addAction(a);
	m_scene = dock;
	m_scene->setWidget(m_renderWidget);

	// Inspector
	dock = new QDockWidget(tr("Inspector"), m_window);
	m_menu->addAction(dock->toggleViewAction());
	m_window->addDockWidget(Qt::RightDockWidgetArea, dock);

	// Hierarchy
	dock = new QDockWidget(tr("Hierarchy"), m_window);
	m_menu->addAction(dock->toggleViewAction());
	m_window->addDockWidget(Qt::RightDockWidgetArea, dock);

	// Maximize scene
	a = new QAction("Maximize Scene", this);
	a->setCheckable(true);
	a->setShortcut(QKeySequence("Ctrl+G"));
	m_menu->addAction(a);
	connect(a, SIGNAL(toggled(bool)), this, SLOT(setMaximizeScene(bool)));

	// Fullscreen
	a = new QAction("Fullscreen", this);
	a->setCheckable(true);
	a->setShortcut(QKeySequence("F1"));
	connect(a, SIGNAL(toggled(bool)), m_window, SLOT(setFullscreen(bool)));
	m_menu->addAction(a);
}

void Manager_Docks::createDockWidget()
{
	QDockWidget* dock;
	dock = new QDockWidget(tr("Foo"), m_window);
	m_menu->addAction(dock->toggleViewAction());
	m_window->addDockWidget(Qt::RightDockWidgetArea, dock);
}

void Manager_Docks::setMaximizeScene( bool p_checked )
{
	if(p_checked)
	{
		m_scene->setFloating(true);
		m_scene->showFullScreen();
	}
	else
	{
		m_scene->setFloating(false);
		m_scene->showNormal();
	}
}
