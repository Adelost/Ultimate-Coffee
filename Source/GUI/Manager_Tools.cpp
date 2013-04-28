#include "Manager_Tools.h"

#include "Window.h"
#include "Util.h"
#include "ui_MainWindow.h"

void Manager_Tools::init()
{
	m_window = Window::instance();
	m_ui = m_window->ui();
	setupActions();
	setupToolbar();
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

	// Quit
	a = m_ui->actionQuit;
	a->setShortcut(QKeySequence("Ctrl+Q"));
	a->setStatusTip(tr("Quit application"));
	connect(a, SIGNAL(triggered()), m_window, SLOT(close()));

	// HELP					
	// About
	a = new QAction("&About", this);
	a->setStatusTip("Show application info");
	connect(a, SIGNAL(triggered()), this, SLOT(action_about()));
	m_ui->menuHelp->addAction(a);
	// About Qt
	a = new QAction(tr("About &Qt"), this);
	a->setStatusTip(tr("This GUI was made in Qt"));
	connect(a, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	m_ui->menuHelp->addAction(a);
}

void Manager_Tools::setupActions()
{
	QAction* a;

	// Toolbar
	toolGroup = new QActionGroup(this);
	m_ui->toolBar->setIconSize(QSize(18,18));
	createToolAction("Translate")->setChecked(true);
	createToolAction("Rotate");
	createToolAction("Scale");
	createToolAction("Geometry");
	createToolAction("Entity");

	// Context bar
	m_ui->contextBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
	QMenu* menu;
	menu = new QMenu("Info", m_window);
	menu->setIcon(createIcon("Translate"));
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
		"Coffee... is a feeling.");
}

QAction* Manager_Tools::createContextIcon( std::string p_icon )
{
	std::string path = "";
	path = path + ICON_PATH + "Tools/" + p_icon;
	
	QAction* a = new QAction(QIcon(path.c_str()), p_icon.c_str(), m_window);
	m_ui->contextBar->addAction(a);

	return a;
}

QAction* Manager_Tools::createToolAction( std::string p_icon )
{
	std::string path = "";
	path = path + ICON_PATH + "Tools/" + p_icon;

	QAction* a = new QAction(QIcon(path.c_str()), p_icon.c_str(), m_window);
	a->setCheckable(true);
	toolGroup->addAction(a);
	m_ui->toolBar->addAction(a);

	return a;
}

QIcon Manager_Tools::createIcon( std::string p_icon )
{
	std::string path = "";
	path = path + ICON_PATH + "Tools/" + p_icon;

	return QIcon(path.c_str());
}

