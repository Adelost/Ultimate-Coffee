#include "stdafx.h"
#include "Manager_Commands.h"

#include <Core/World.h> //SETTINGS
#include <Core/Commander.h>
#include <Core/Command_ChangeBackBufferColor.h>
#include <Core/Events.h>
#include "ui_MainWindow.h"
#include "Manager_Docks.h"
#include "Window.h"

void Manager_Commands::init()
{
	SUBSCRIBE_TO_EVENT(this, EVENT_STORE_COMMAND);
	m_window = Window::instance();
	m_ui = m_window->ui();

	m_action_undo = NULL;
	m_action_redo = NULL;
	//nrOfSoundsPlayedSinceLastReset = 0;

	// Init undo/redo system
	m_commander = new Commander();
	if(!m_commander->init())
	{
		MESSAGEBOX("Commander failed to init.");
	}
	m_lastValidProjectPath = "";


	setupMenu();
}

void Manager_Commands::setupMenu()
{
	QAction* a;
	std::string iconPath = ICON_PATH;
	std::string path;

	//
	// Add toolbar
	//

	m_toolbar_commands = new QToolBar(m_window);
	m_window->addToolBar(Qt::TopToolBarArea, m_toolbar_commands);


	//
	// Add menu
	//

	// Save/Load

	// Open
	a = m_ui->actionOpen;
	/*path = iconPath + "Menu/open";
	a->setIcon(,(path.c_str())); */
	a->setStatusTip(tr("Open existing project"));
	a->setShortcuts(QKeySequence::Open);
	connect(a, SIGNAL(triggered()), this, SLOT(loadCommandHistory()));

	// Save
	a = m_ui->actionSave;
	/*path = iconPath + "Menu/save";
	a->setIcon(QIcon(path.c_str())); */
	a->setShortcuts(QKeySequence::Save);
	a->setStatusTip(tr("Save project"));
	connect(a, SIGNAL(triggered()), this, SLOT(saveCommandHistory()));

	// Save as
	a = m_ui->actionSave_As;
	a->setStatusTip(tr("Save project to..."));
	connect(a, SIGNAL(triggered()), this, SLOT(saveCommandHistoryAs()));

	
	// Undo
	a = m_ui->actionUndo;
	m_action_undo = a;
	/*path = iconPath + "Menu/undo";
	a->setIcon(QIcon(path.c_str())); */
	a->setShortcuts(QKeySequence::Undo); 
	a->setStatusTip(tr("Undo last editing action"));
	connect(a, SIGNAL(triggered()), this, SLOT(undoLatestCommand()));

	// Redo
	a = m_ui->actionRedo;
	m_action_undo = a;
	/*path = iconPath + "Menu/redo";
	a->setIcon(QIcon(path.c_str())); */
	a->setShortcuts(QKeySequence::Redo); 
	a->setStatusTip(tr("Redo last undone editing action"));
	connect(a, SIGNAL(triggered()), this, SLOT(redoLatestCommand()));


	//
	// Add buttons
	//

	// A signal Mapper makes it easier call a slot with
	// different parameters
	QSignalMapper* mapper = new QSignalMapper(this);
	connect(mapper, SIGNAL(mapped(QString)), this, SLOT(setBackBufferColor(QString)));

	// Setup actions
	createTestButton("#000", mapper);
	createTestButton("#f00", mapper);
	createTestButton("#0f0", mapper);
	createTestButton("#00f", mapper);
	createTestButton("#0ff", mapper);
	createTestButton("#ff0", mapper);
	createTestButton("#f0f", mapper);
}

void Manager_Commands::setBackBufferColor(QString p_str_color)
{
	QColor color = (p_str_color);
	Command_ChangeBackBufferColor* command = new Command_ChangeBackBufferColor();
	command->setDoColor(color.red(), color.green(), color.blue());
	command->setUndoColor(SETTINGS()->backBufferColor.x, SETTINGS()->backBufferColor.y, SETTINGS()->backBufferColor.z);
	
	SEND_EVENT(&Event_StoreCommand(command, true));
}

Manager_Commands::~Manager_Commands()
{
	delete m_commander;
}

void Manager_Commands::loadCommandHistory()
{
	// Opens standard Windows "open file" dialog
	QString fileName = QFileDialog::getOpenFileName(m_window, tr("Open Ultimate Coffee Project"), "UltimateCoffeeProject.uc", tr("Ultimate Coffee Project (*.uc)"));

	// If the user clicks "Open"
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

void Manager_Commands::redoLatestCommand()
{
	if(!m_commander->tryToRedoLatestUndoCommand())
	{
		QSound sound = QSound("Windows Ding.wav");
		sound.play();
	}
}

void Manager_Commands::saveCommandHistory()
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

void Manager_Commands::saveCommandHistoryAs()
{
	//Opens standard Windows "save file" dialog
	QString fileName = QFileDialog::getSaveFileName(m_window, tr("Save Ultimate Coffee Project"), "UltimateCoffeeProject", tr("Ultimate Coffee Project (*.uc)"));

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

void Manager_Commands::undoLatestCommand()
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

void Manager_Commands::createTestButton( QString color, QSignalMapper* mapper )
{
	QAction* a = new QAction(m_window->createIcon(&QColor(color)), color, m_window);
	m_toolbar_commands->addAction(a);
	connect(a, SIGNAL(triggered()), mapper, SLOT(map()));
	mapper->setMapping(a, color);
}

void Manager_Commands::onEvent(IEvent* e)
{
	EventType type = e->type();
	switch (type)
	{
	case EVENT_STORE_COMMAND: //Add a command, sent in an event, to the commander. It might also be executed.
		Event_StoreCommand* commandEvent = static_cast<Event_StoreCommand*>(e);
		Command* command = commandEvent->command;
		if(commandEvent->execute)
		{
			m_commander->addToHistoryAndExecute(command);
		}
		else
		{
			m_commander->addToHistory(command);
		}

		SEND_EVENT(&Event_ShowInGUI(command)); //Update command history in GUI
		break;
	}
}