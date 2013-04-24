#include "MainWindow.h"

// Qt
#include <QMessageBox.h>
#include <QFileDialog>
#include <QLabel.h>
#include <QGraphicsBlurEffect>
#include <QShortcut.h>
#include <QSound.h>
#include <QTimer.h>

// Application
#include <Core/EventManager.h>

// Commands
#include <Core/Commander.h>
#include <Core/Command_ChangeBackBufferColor.h>

// Stuff used to allocate console
// no idea what most of it does
#include <io.h>
#include <fcntl.h>
#include <Windows.h>

// Normal includes
#include "UpdateLoop.h"

MainWindow::MainWindow()
{
	m_action_undo = NULL;
	m_action_redo = NULL;
	//nrOfSoundsPlayedSinceLastReset = 0;
	
	// Init achitecture
	Math::init();
	SUBSCRIBE_TO_EVENT(this, EVENT_SHOW_MESSAGEBOX);

	// Init window
	m_ui.setupUi(this);
	m_renderWidget = new RenderWidget(this);
	setWindowTitle("Ultimate Coffee");
	setupToolBar();
	setupDockWidgets();
	setDockOptions(AllowNestedDocks | AllowTabbedDocks);
	//setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North); // Flip tab location

	// Init game
	setupConsole();
	m_updateLoop = new UpdateLoop();
	m_updateLoop->init();
	setupGame();

	// Start update loop
	m_updateTimer.reset();
	m_refreshTimer.setInterval(0);
	connect(&m_refreshTimer, SIGNAL(timeout()), this, SLOT(update()));
	m_refreshTimer.start();
}

MainWindow::~MainWindow()
{
	delete m_commander;
	delete m_updateLoop;
}

void MainWindow::setupGame()
{
	// Init undo/redo system
	m_commander = new Commander();
	if(!m_commander->init())
	{
		MESSAGEBOX("Commander failed to init.");
	}

	m_lastValidProjectPath = "";
}


void MainWindow::update()
{
	computeFPS();
	m_updateTimer.tick();
	SETTINGS()->deltaTime = m_updateTimer.deltaTime();
	m_updateLoop->update();

    //connect(&soundTimer, SIGNAL(timeout()), this, SLOT(timer()));
    //soundTimer.start(1000);

	//if(commander->redoIsPossible())
	//{
	//	redo->setDisabled(false);
	//}
	//else
	//{
	//	redo->setDisabled(true);
	//}

	//if(commander->undoIsPossible())
	//{
	//	undo->setDisabled(false);
	//}
	//else
	//{
	//	undo->setDisabled(true);
	//}
}

void MainWindow::setupToolBar()
{
	// Actions
	QAction* a;
	std::string iconPath = ICON_PATH;
	std::string path;
	

	// FILE
	// Save
	path = iconPath + "Menu/new";
	a = new QAction(QIcon(path.c_str()), "&New", this);
	a->setShortcuts(QKeySequence::New);
	a->setStatusTip(tr("NO FUNCTIONALITY YET (2013-04-24, 14.51)"));
	m_ui.menuFile->addAction(a);
	// Open
	path = iconPath + "Menu/open";
	a = new QAction(QIcon(path.c_str()), "&Open...", this);
	a->setStatusTip(tr("Open existing project"));
	a->setShortcuts(QKeySequence::Open);
	m_ui.menuFile->addAction(a);

	connect(a, SIGNAL(triggered()), this, SLOT(loadCommandHistory()));

	// Save
	path = iconPath + "Menu/save";
	a = new QAction(QIcon(path.c_str()), "&Save", this);
	a->setShortcuts(QKeySequence::Save);
	a->setStatusTip(tr("Save project"));
	m_ui.menuFile->addAction(a);

	connect(a, SIGNAL(triggered()), this, SLOT(saveCommandHistory()));

	// Save as
	a = new QAction("&Save As...", this);
	a->setStatusTip(tr("Save project to..."));
	m_ui.menuFile->addAction(a);

	connect(a, SIGNAL(triggered()), this, SLOT(saveCommandHistoryAs()));

	// Quit
	a = new QAction("&Quit", this);
	a->setShortcut(QKeySequence("Ctrl+Q"));
	a->setStatusTip(tr("Quit application"));
	connect(a, SIGNAL(triggered()), this, SLOT(close()));
	//ui.menuFile->addSeparator();
	m_ui.menuFile->addAction(a);

	// EDIT
	// Undo
	path = iconPath + "Menu/undo";
	a = new QAction(QIcon(path.c_str()), tr("&Undo"), this);
	a->setShortcuts(QKeySequence::Undo);
	a->setStatusTip(tr("Undo last editing action"));
	
	m_action_undo = a;
	//undo->setDisabled(true);
	m_ui.menuEdit->addAction(a);

	connect(a, SIGNAL(triggered()), this, SLOT(undoLatestCommand()));

	// Redo
	a = new QAction(tr("&Redo"), this);
	a->setShortcuts(QKeySequence::Redo);
	a->setStatusTip(tr("Redo last undone editing action"));
	
	m_action_redo = a;
	//redo->setDisabled(true);
	m_ui.menuEdit->addAction(a);

	connect(a, SIGNAL(triggered()), this, SLOT(redoLatestCommand()));
	
	// HELP					
	// About
	a = new QAction("&About", this);
	a->setStatusTip("Show application info");
	connect(a, SIGNAL(triggered()), this, SLOT(action_about()));
	m_ui.menuHelp->addAction(a);
	// About Qt
	a = new QAction(tr("About &Qt"), this);
	a->setStatusTip(tr("This GUI was made in Qt"));
	connect(a, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	m_ui.menuHelp->addAction(a);

	// Toolbar
	//ui.toolBar->setAllowedAreas(Qt::LeftToolBarArea | Qt::RightToolBarArea);
	path = iconPath + "Tools/translate";
	a = new QAction(QIcon(path.c_str()), tr("&Translate"), this);
	a->setCheckable(true);
	a->setChecked(true);
	m_ui.toolBar->addAction(a);
	path = iconPath + "Tools/rotate";
	a = new QAction(QIcon(path.c_str()), tr("&Rotate"), this);
	a->setCheckable(true);
	m_ui.toolBar->addAction(a);
	path = iconPath + "Tools/scale";
	a = new QAction(QIcon(path.c_str()), tr("&Scale"), this);
	a->setCheckable(true);
	m_ui.toolBar->addAction(a);
	path = iconPath + "Tools/geometry";
	a = new QAction(QIcon(path.c_str()), tr("&Geometry"), this);
	a->setCheckable(true);
	m_ui.toolBar->addAction(a);
	path = iconPath + "Tools/entity";
	a = new QAction(QIcon(path.c_str()), tr("&Entity"), this);
	a->setCheckable(true);
	m_ui.toolBar->addAction(a);


	// Context bar
	m_ui.contextBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
	path = iconPath + "Tools/translate";
	a = new QAction(QIcon(path.c_str()), tr("info"), this);
	m_ui.contextBar->addAction(a);
	m_ui.contextBar->addSeparator();
	path = iconPath + "Tools/toast";
	a = new QAction(QIcon(path.c_str()), tr("toast"), this);
	m_ui.contextBar->addAction(a);
	
	//TEMP
	connect(a, SIGNAL(triggered()), this, SLOT(setBackBufferColorToRed()));

	path = iconPath + "Tools/coffee";
	a = new QAction(QIcon(path.c_str()), tr("coffee"), this);
	m_ui.contextBar->addAction(a);

	//TEMP
	connect(a, SIGNAL(triggered()), this, SLOT(setBackBufferColorToGreen()));

	path = iconPath + "Tools/wine";
	a = new QAction(QIcon(path.c_str()), tr("wine"), this);
	m_ui.contextBar->addAction(a);

	//TEMP
	connect(a, SIGNAL(triggered()), this, SLOT(setBackBufferColorToBlue()));

	path = iconPath + "Tools/experiment";
	a = new QAction(QIcon(path.c_str()), tr("experiment"), this);
	m_ui.contextBar->addAction(a);
	path = iconPath + "Tools/tool";
	a = new QAction(QIcon(path.c_str()), tr("tool"), this);
	m_ui.contextBar->addAction(a);

	// DOCK WIDGETS
	this->centralWidget()->hide();
	QDockWidget* dock;
	dock = new QDockWidget(tr("Scene"), this);
	m_sceneDock = dock;
	a = dock->toggleViewAction();
	m_ui.menuWindow->addAction(a);
	addDockWidget(Qt::LeftDockWidgetArea, dock);
	dock->setWidget(m_renderWidget);

	dock = new QDockWidget(tr("Inspector"), this);
	m_ui.menuWindow->addAction(dock->toggleViewAction());
	addDockWidget(Qt::RightDockWidgetArea, dock);
	dock = new QDockWidget(tr("Hierarchy"), this);
	m_ui.menuWindow->addAction(dock->toggleViewAction());
	addDockWidget(Qt::RightDockWidgetArea, dock);
	//dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

	// WINDOW
	a = new QAction("Fullscreen", this);
	a->setCheckable(true);
	a->setShortcut(QKeySequence("F1"));
	connect(a, SIGNAL(toggled(bool)), this, SLOT(setFullscreen(bool)));
	m_ui.menuWindow->addAction(a);
	a = new QAction("Maximize Scene", this);
	a->setCheckable(true);
	a->setShortcut(QKeySequence("Ctrl+G"));
	m_ui.menuWindow->addAction(a);
	connect(a, SIGNAL(toggled(bool)), this, SLOT(setMaximizeScene(bool)));


	a = new QAction("Create Dock", this);
	a->setShortcuts(QKeySequence::AddTab);
	connect(a, SIGNAL(triggered()), this, SLOT(createDockWidget()));
	m_ui.menuWindow->addAction(a);

	// BLARG

}


void MainWindow::setupDockWidgets()
{
	/*for (int i=0; i<3; i++)
	{
	ToolBar *tb = new ToolBar(QString::fromLatin1("Tool Bar %1").arg(i + 1), this);
	toolBars.append(tb);
	addToolBar(tb);
	}*/

	/*QMenu* corner_menu = dockWidgetMenu->addMenu(tr("Top left corner"));
	QActionGroup *group = new QActionGroup(this);
	group->setExclusive(true);
	::addAction(corner_menu, tr("Top dock area"), group, mapper, 0);
	::addAction(corner_menu, tr("Left dock area"), group, mapper, 1);*/
}




void MainWindow::setupConsole()
{
	// Create debug console
	AllocConsole();
	SetConsoleTitle(L"Debug console");
	int hConHandle;
	long lStdHandle;
	FILE *fp;   // redirect unbuffered STDOUT to the console
	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );

	// Test print
	DEBUGPRINT("////////////////////");
	DEBUGPRINT("  								");
  	DEBUGPRINT("  INITIALIZING					");
	DEBUGPRINT("  Debug Console					");
	DEBUGPRINT("  								");
	DEBUGPRINT("    ~-_	  Steamin'				");
	DEBUGPRINT("     _-~    Hot					");
	DEBUGPRINT("   c|_|	  COFFEE				");
	DEBUGPRINT("  								");
	DEBUGPRINT("////////////////////");
	DEBUGPRINT("  								");
}																				

void MainWindow::computeFPS()
{
	static int num_frames = 0;
	static float timeElapsed = 0.0f;
	num_frames++;

	// Compute averages FPS and ms over one second period.
	if((m_updateTimer.totalTime()-timeElapsed) >= 1.0f)
	{
		// calculate statistics
		float fps = (float)num_frames; // fps = frameCnt / 1
		float msPerFrame = 1000.0f/fps;

		// convert statistics into QString
		QString stats;
		stats = "FPS:  %1  Frame Time:  %2 (ms)";
		stats = stats.arg(fps).arg((int)msPerFrame);

		// send signal

		//static int debugId = StopWatch::getUniqueId();
		//SEND_EVENT(&Event_PostDebugMessage(debugId, "Total: " + Converter::IntToStr(fps) + " (fps)"));

		//statusBar()->showMessage(stats);

		setWindowTitle("Ultimate Coffee - " + stats);

		// reset stats for next average.
		num_frames = 0;
		timeElapsed += 1.0f;
	}
}

void MainWindow::action_about()
{
	QMessageBox::about(this, "About Ultimate Coffee",
		"Coffee... is a feeling.");
}

void MainWindow::setBackBufferColorToRed()
{
	Command_ChangeBackBufferColor* red = new Command_ChangeBackBufferColor();
	red->setDoColor(1.0f, 0.0f, 0.0f);
	red->setUndoColor(SETTINGS()->backBufferColor.x, SETTINGS()->backBufferColor.y, SETTINGS()->backBufferColor.z);
	m_commander->addToHistoryAndExecute(red);
}

void MainWindow::setBackBufferColorToGreen()
{
	Command_ChangeBackBufferColor* green = new Command_ChangeBackBufferColor();
	green->setDoColor(0.0f, 1.0f, 0.0f);
	green->setUndoColor(SETTINGS()->backBufferColor.x, SETTINGS()->backBufferColor.y, SETTINGS()->backBufferColor.z);
	m_commander->addToHistoryAndExecute(green);
}

void MainWindow::setBackBufferColorToBlue()
{
	Command_ChangeBackBufferColor* blue = new Command_ChangeBackBufferColor();
	blue->setDoColor(0.0f, 0.0f, 1.0f);
	blue->setUndoColor(SETTINGS()->backBufferColor.x, SETTINGS()->backBufferColor.y, SETTINGS()->backBufferColor.z);
	m_commander->addToHistoryAndExecute(blue);
}

void MainWindow::undoLatestCommand()
{
	if(!m_commander->tryToUndoLatestCommand())
	{
		QSound sound = QSound("Windows Ding.wav");
		//if(nrOfSoundsPlayedSinceLastReset < 1)
		//{
			sound.play();
		//}
		//nrOfSoundsPlayedSinceLastReset++;
	}
}

void MainWindow::redoLatestCommand()
{
	if(!m_commander->tryToRedoLatestUndoCommand())
	{
		QSound sound = QSound("Windows Ding.wav");
		sound.play();
	}
}

void MainWindow::loadCommandHistory()
{
	//Opens standard Windows "open file" dialog
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Ultimate Coffee Project"), "UltimateCoffeeProject.uc", tr("Ultimate Coffee Project (*.uc)"));
	
	//If the user clicks "Open"
	if(!fileName.isEmpty())
	{
		std::string path = fileName.toLocal8Bit();
		if(!m_commander->tryToLoadCommandHistory(path))
		{
			MESSAGEBOX("Failed to load project. Please contact Folke Peterson-Berger.");
		}
		else
		{
			m_lastValidProjectPath = path;
		}
	}
}

void MainWindow::saveCommandHistory()
{
	if(m_lastValidProjectPath == "")
	{
		saveCommandHistoryAs();
	}
	else
	{
		if(!m_commander->tryToSaveCommandHistory(m_lastValidProjectPath))
		{
			MESSAGEBOX("Failed to save project.");
		}
	}
}

void MainWindow::saveCommandHistoryAs()
{
	//Opens standard Windows "save file" dialog
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Ultimate Coffee Project"), "UltimateCoffeeProject", tr("Ultimate Coffee Project (*.uc)"));

	//If the user clicks "Save"
	if(!fileName.isEmpty())
	{
		std::string path = fileName.toLocal8Bit();
		if(!m_commander->tryToSaveCommandHistory(path))
		{
			MESSAGEBOX("Failed to save project.");
		}
		else
		{
			m_lastValidProjectPath = path;
		}
	}
}

void MainWindow::setFullscreen( bool checked )
{
	if(checked)
		this->showFullScreen();
	else
		this->showNormal();
}

void MainWindow::createDockWidget()
{
	QDockWidget* dock;
	dock = new QDockWidget(tr("Foo"), this);
	m_ui.menuWindow->addAction(dock->toggleViewAction());
	addDockWidget(Qt::RightDockWidgetArea, dock);
}

void MainWindow::onEvent( IEvent* p_event )
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

void MainWindow::setMaximizeScene( bool p_checked )
{
	if(p_checked)
	{
		m_sceneDock->setFloating(true);
		m_sceneDock->showFullScreen();
	}
	else
	{
		m_sceneDock->setFloating(false);
		m_sceneDock->showNormal();
	}
}
//void MainWindow::timer()
//{
//	nrOfSoundsPlayedSinceLastReset = 0;
//}
