#include "MainWindow.h"

//Qt
#include <QFileDialog>
#include <QLabel.h>
#include <QGraphicsBlurEffect>

// Architecture
#include <Core/World.h>
#include <System_Render/System_Render.h>

//Commands
#include <Core/Commander.h>
#include <Core/Command_ChangeBackBufferColor.h>

// Stuff used to allocate console
// no idea what most of it does
#include <io.h>
#include <fcntl.h>
#include <Windows.h>

MainWindow::MainWindow()
{
	// Init window
	ui.setupUi(this);
	renderWidget = new RenderWidget(this);
	setWindowTitle("Ultimate Coffee");
	setupToolBar();
	setupDockWidgets();
	setDockOptions(AllowNestedDocks | AllowTabbedDocks);
	//setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North); // Flip tab location

	// Init game
	setupConsole();
	setupGame();

	// Start update loop
	updateTimer.reset();
	refreshTimer.setInterval(0);
	connect(&refreshTimer, SIGNAL(timeout()), this, SLOT(update()));
	refreshTimer.start();
}

MainWindow::~MainWindow()
{
	delete commander; //check crash here
}

void MainWindow::setupGame()
{
	// Init systems
	world = WORLD();
	world->addSystem(new System::Translation());
	world->addSystem(new System::Render());

	// Create Entities
	Entity e(0);
	e.fetchData<Data::Position>();
	e.removeData<Data::Position>();
	e.addData(Data::Position());

	// Init undo/redo system
	commander = new Commander();
	if(!commander->init())
	{
		//check, print fail message
	}

	lastValidProjectPath = "";
}


void MainWindow::update()
{
	// Update game
	computeFPS();
	updateTimer.tick();
	SETTINGS()->deltaTime = updateTimer.deltaTime();
	world->update();
	commander->update();
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
	ui.menuFile->addAction(a);
	// Open
	path = iconPath + "Menu/open";
	a = new QAction(QIcon(path.c_str()), "&Open...", this);
	a->setShortcuts(QKeySequence::Open);
	ui.menuFile->addAction(a);

	connect(a, SIGNAL(triggered()), this, SLOT(loadCommandHistory()));

	// Save
	path = iconPath + "Menu/save";
	a = new QAction(QIcon(path.c_str()), "&Save", this);
	a->setShortcuts(QKeySequence::Save);
	ui.menuFile->addAction(a);

	connect(a, SIGNAL(triggered()), this, SLOT(saveCommandHistory()));

	// Save as
	a = new QAction("&Save As...", this);
	ui.menuFile->addAction(a);

	connect(a, SIGNAL(triggered()), this, SLOT(saveCommandHistoryAs()));

	// Quit
	a = new QAction("&Quit", this);
	a->setShortcuts(QKeySequence::Quit);
	a->setStatusTip(tr("Quit the application"));
	connect(a, SIGNAL(triggered()), this, SLOT(close()));
	//ui.menuFile->addSeparator();
	ui.menuFile->addAction(a);

	// EDIT
	// Undo
	path = iconPath + "Menu/undo";
	a = new QAction(QIcon(path.c_str()), tr("&Undo"), this);
	a->setShortcuts(QKeySequence::Undo);
	a->setStatusTip(tr("Undo the last editing action"));
	ui.menuEdit->addAction(a);

	connect(a, SIGNAL(triggered()), this, SLOT(undoLatestCommand()));

	// Redo
	a = new QAction(tr("&Redo"), this);
	a->setShortcuts(QKeySequence::Redo);
	a->setStatusTip(tr("Redo editing action"));
	ui.menuEdit->addAction(a);

	connect(a, SIGNAL(triggered()), this, SLOT(redoLatestCommand()));
	
	// HELP					
	// About
	a = new QAction("&About", this);
	a->setStatusTip("Show the application's About box");
	connect(a, SIGNAL(triggered()), this, SLOT(act_about()));
	ui.menuHelp->addAction(a);
	// About Qt
	a = new QAction(tr("About &Qt"), this);
	a->setStatusTip(tr("Show the Qt library's About box"));
	connect(a, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	ui.menuHelp->addAction(a);

	// Toolbar
	//ui.toolBar->setAllowedAreas(Qt::LeftToolBarArea | Qt::RightToolBarArea);
	path = iconPath + "Tools/translate";
	a = new QAction(QIcon(path.c_str()), tr("&Translate"), this);
	a->setCheckable(true);
	a->setChecked(true);
	ui.toolBar->addAction(a);
	path = iconPath + "Tools/rotate";
	a = new QAction(QIcon(path.c_str()), tr("&Rotate"), this);
	a->setCheckable(true);
	ui.toolBar->addAction(a);
	path = iconPath + "Tools/scale";
	a = new QAction(QIcon(path.c_str()), tr("&Scale"), this);
	a->setCheckable(true);
	ui.toolBar->addAction(a);
	path = iconPath + "Tools/geometry";
	a = new QAction(QIcon(path.c_str()), tr("&Geometry"), this);
	a->setCheckable(true);
	ui.toolBar->addAction(a);
	path = iconPath + "Tools/entity";
	a = new QAction(QIcon(path.c_str()), tr("&Entity"), this);
	a->setCheckable(true);
	ui.toolBar->addAction(a);

	// Context bar
	ui.contextBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
	path = iconPath + "Tools/translate";
	a = new QAction(QIcon(path.c_str()), tr("info"), this);
	ui.contextBar->addAction(a);
	ui.contextBar->addSeparator();
	path = iconPath + "Tools/toast";
	a = new QAction(QIcon(path.c_str()), tr("toast"), this);
	ui.contextBar->addAction(a);
	
	//TEMP
	connect(a, SIGNAL(triggered()), this, SLOT(setBackBufferColorToRed()));

	path = iconPath + "Tools/coffee";
	a = new QAction(QIcon(path.c_str()), tr("coffee"), this);
	ui.contextBar->addAction(a);

	//TEMP
	connect(a, SIGNAL(triggered()), this, SLOT(setBackBufferColorToGreen()));

	path = iconPath + "Tools/wine";
	a = new QAction(QIcon(path.c_str()), tr("wine"), this);
	ui.contextBar->addAction(a);

	//TEMP
	connect(a, SIGNAL(triggered()), this, SLOT(setBackBufferColorToBlue()));

	path = iconPath + "Tools/experiment";
	a = new QAction(QIcon(path.c_str()), tr("experiment"), this);
	ui.contextBar->addAction(a);
	path = iconPath + "Tools/tool";
	a = new QAction(QIcon(path.c_str()), tr("tool"), this);
	ui.contextBar->addAction(a);

	// DOCK WIDGETS
	this->centralWidget()->hide();
	QDockWidget* dock;
	dock = new QDockWidget(tr("Scene"), this);
	ui.menuWindow->addAction(dock->toggleViewAction());
	addDockWidget(Qt::LeftDockWidgetArea, dock);
	dock->setWidget(renderWidget);

	dock = new QDockWidget(tr("Inspector"), this);
	ui.menuWindow->addAction(dock->toggleViewAction());
	addDockWidget(Qt::RightDockWidgetArea, dock);
	dock = new QDockWidget(tr("Hierarchy"), this);
	ui.menuWindow->addAction(dock->toggleViewAction());
	addDockWidget(Qt::RightDockWidgetArea, dock);
	//dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

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
	if((updateTimer.totalTime()-timeElapsed) >= 1.0f)
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

		statusBar()->showMessage(stats);

		// reset stats for next average.
		num_frames = 0;
		timeElapsed += 1.0f;
	}
}

void MainWindow::act_about()
{
	QMessageBox::about(this, "About Ultimate Coffee",
		"Coffee... is a feeling.");
}

void MainWindow::setBackBufferColorToRed()
{
	Command_ChangeBackBufferColor* red = new Command_ChangeBackBufferColor();
	red->setDoColor(1.0f, 0.0f, 0.0f);
	red->setUndoColor(SETTINGS()->backBufferColorX, SETTINGS()->backBufferColorY, SETTINGS()->backBufferColorZ);
	commander->addToHistoryAndExecute(red);
}

void MainWindow::setBackBufferColorToGreen()
{
	Command_ChangeBackBufferColor* green = new Command_ChangeBackBufferColor();
	green->setDoColor(0.0f, 1.0f, 0.0f);
	green->setUndoColor(SETTINGS()->backBufferColorX, SETTINGS()->backBufferColorY, SETTINGS()->backBufferColorZ);
	commander->addToHistoryAndExecute(green);
}

void MainWindow::setBackBufferColorToBlue()
{
	Command_ChangeBackBufferColor* blue = new Command_ChangeBackBufferColor();
	blue->setDoColor(0.0f, 0.0f, 1.0f);
	blue->setUndoColor(SETTINGS()->backBufferColorX, SETTINGS()->backBufferColorY, SETTINGS()->backBufferColorZ);
	commander->addToHistoryAndExecute(blue);
}

void MainWindow::undoLatestCommand()
{
	if(!commander->tryToUndoLatestCommand())
	{
		//check, add feedback
	}
}

void MainWindow::redoLatestCommand()
{
	if(!commander->tryToRedoLatestUndoCommand())
	{
		//check, add feedback
	}
}

void MainWindow::loadCommandHistory()
{
	//Opens standard Windows "open file" dialog
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Ultimate Coffee Project"), "UltimateCoffeeProject", tr("Ultimate Coffee Project (*.uc)"));
	
	//If the user clicks "Open"
	if(!fileName.isEmpty())
	{
		std::string path = fileName.toLocal8Bit();
		if(!commander->tryToLoadCommandHistory(path))
		{
			//check, add error feedback
		}
		else
		{
			lastValidProjectPath = path;
		}
	}
}

void MainWindow::saveCommandHistory()
{
	if(lastValidProjectPath == "")
	{
		saveCommandHistoryAs();
	}
	else
	{
		if(!commander->tryToSaveCommandHistory(lastValidProjectPath))
		{
			//check, add error feedback
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
		if(!commander->tryToSaveCommandHistory(path))
		{
			//check, add error feedback
		}
		else
		{
			lastValidProjectPath = path;
		}
	}
}