#include "stdafx.h"
#include "Manager_Docks.h"
#include "RenderWidget.h"
#include "Window.h"
#include "ui_MainWindow.h"


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
	setupHierarchy();
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
	m_window->centralWidget()->hide();

	// Add dock action
	QAction* a;
	a = new QAction("Create Dock", m_window);
	a->setShortcuts(QKeySequence::AddTab);
	connect(a, SIGNAL(triggered()), this, SLOT(createDockWidget()));
	m_menu->addAction(a);
	connect(m_window->ui()->actionReset_Layout, SIGNAL(triggered()), this, SLOT(resetLayout()));
	connect(m_window->ui()->actionSave_Layout, SIGNAL(triggered()), this, SLOT(saveLayout()));
	connect(m_window->ui()->actionLoad_Layout, SIGNAL(triggered()), this, SLOT(loadLayout()));

	QDockWidget* dock;

	// Scene
	dock = createDock("Scene", Qt::LeftDockWidgetArea);
	dock->setWidget(m_renderWidget);
	m_scene = dock;

	// Inspector
	dock = createDock("Inspector", Qt::RightDockWidgetArea);

	// Hierarchy
	dock = createDock("Hierarchy", Qt::RightDockWidgetArea);
	QTreeView* tree = new QTreeView(m_window);
	m_hierarchy = new QStandardItemModel(0, 1, this);
	m_hierarchy->setHorizontalHeaderItem(0, new QStandardItem("Entity ID"));
	tree->setModel(m_hierarchy);
	dock->setWidget(tree);

	// Maximize scene
	a = createAction("Maximize Scene");
	a->setCheckable(true);
	a->setShortcut(QKeySequence("Ctrl+G"));
	connect(a, SIGNAL(toggled(bool)), this, SLOT(setMaximizeScene(bool)));

	// Fullscreen
	a = createAction("Fullscreen");
	a->setCheckable(true);
	a->setShortcut(QKeySequence("F1"));
	connect(a, SIGNAL(toggled(bool)), m_window, SLOT(setFullscreen(bool)));
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

QAction* Manager_Docks::createAction( QString p_name )
{
	QAction* a;
	a = new QAction(p_name, m_window);
	a->setObjectName(p_name);
	m_menu->addAction(a);

	return a;
}

QDockWidget* Manager_Docks::createDock( QString p_name, Qt::DockWidgetArea p_area )
{
	QDockWidget* dock = new QDockWidget(p_name, m_window);
	dock->setObjectName(p_name);
	m_menu->addAction(dock->toggleViewAction());
	m_window->addDockWidget(p_area, dock);

	return dock;
}

void Manager_Docks::resetLayout()
{
	QString fileName = "layout_default";
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

void Manager_Docks::setupHierarchy()
{
	for(int i=0; i<5; i++)
	{
		QStandardItem* item;
		item = new QStandardItem("Foo");
		m_hierarchy->appendRow(item);
	}
}
