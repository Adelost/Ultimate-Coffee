#include "stdafx.h"
#include "Manager_Commands.h"

#include <Core/World.h> //SETTINGS
#include <Core/Commander.h>
#include <Core/CommanderSpy.h>
#include <Core/Command_ChangeBackBufferColor.h>
#include <Core/Events.h>
#include "ui_MainWindow.h"
#include "Manager_Docks.h"
#include "Window.h"

void Manager_Commands::init()
{
	SUBSCRIBE_TO_EVENT(this, EVENT_STORE_COMMAND);
	SUBSCRIBE_TO_EVENT(this, EVENT_TRACK_TO_COMMAND_HISTORY_INDEX);
	SUBSCRIBE_TO_EVENT(this, EVENT_GET_COMMANDER_INFO);
	m_window = Window::instance();
	m_ui = m_window->ui();

	m_action_undo = nullptr;
	m_action_redo = nullptr;
	//nrOfSoundsPlayedSinceLastReset = 0;

	// Init undo/redo system
	m_commander = new Commander();
	if(!m_commander->init())
	{
		MESSAGEBOX("Commander failed to init.");
	}
	m_commanderSpy = new CommanderSpy(m_commander);
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
	
	SEND_EVENT(&Event_StoreCommandInCommandHistory(command, true));
}

void Manager_Commands::translateSceneEntity()
{

}

Manager_Commands::~Manager_Commands()
{
	delete m_commander;
	delete m_commanderSpy;
}

void Manager_Commands::redoLatestCommand()
{
	if(m_commander->tryToRedoLatestUndoCommand())
	{
		//Update GUI command history
		int GUI_Index = Converter::convertBetweenCommandHistoryIndexAndGUIListIndex(m_commander->getCurrentCommandIndex(), m_commander->getNrOfCommands());
		SEND_EVENT(&Event_SetSelectedCommandGUI(GUI_Index));
	}
	else
	{
		QSound sound = QSound("Windows Ding.wav");
		sound.play();
	}
}

void Manager_Commands::undoLatestCommand()
{
	if(m_commander->tryToUndoLatestCommand())
	{
		//Update GUI command history
		int GUI_Index = Converter::convertBetweenCommandHistoryIndexAndGUIListIndex(m_commander->getCurrentCommandIndex(), m_commander->getNrOfCommands());
		SEND_EVENT(&Event_SetSelectedCommandGUI(GUI_Index));
	}
	else
	{
		QSound sound = QSound("Windows Ding.wav");
		//if(nrOfSoundsPlayedSinceLastReset < 1)
		//{
		sound.play();
		//}
		//nrOfSoundsPlayedSinceLastReset++;
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
		else
		{
			// check, remove if the command list gets very long and takes time to print
			// Prints the command history to the console in an effort to spot bugs (weird values in the printout)
			m_commander->printCommandHistory();
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

void Manager_Commands::loadCommandHistory()
{
	// Opens standard Windows "open file" dialog
	QString fileName = QFileDialog::getOpenFileName(m_window, tr("Open Ultimate Coffee Project"), "UltimateCoffeeProject.uc", tr("Ultimate Coffee Project (*.uc)"));

	// If the user clicks "Open"
	if(!fileName.isEmpty())
	{
		SEND_EVENT(&Event_RemoveCommandsFromCommandHistoryGUI(0, m_commander->getNrOfCommands())); // Clear command history GUI list

		std::string path = fileName.toLocal8Bit();
		if(m_commander->tryToLoadCommandHistory(path))
		{
			m_lastValidProjectPath = path;

			std::vector<Command*>* commands = m_commanderSpy->getCommands();
			int nrOfCommands = commands->size();
			for(int i=0;i<nrOfCommands;i++)
			{
				Command* command = commands->at(i);
				SEND_EVENT(&Event_AddCommandToCommandHistoryGUI(command)); //Add all commands to GUI
			}

			int GUI_Index = Converter::convertBetweenCommandHistoryIndexAndGUIListIndex(m_commander->getCurrentCommandIndex(), m_commander->getNrOfCommands());
			SEND_EVENT(&Event_SetSelectedCommandGUI(GUI_Index));

			// check, remove if the command list gets very long and takes time to print
			// Prints the command history to the console in an effort to spot bugs (weird values in the printout)
			m_commander->printCommandHistory();
		}
		else
		{
			MESSAGEBOX("Failed to load project. Please contact Folke Peterson-Berger.");
		}
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
		{
			Event_StoreCommandInCommandHistory* commandEvent = static_cast<Event_StoreCommandInCommandHistory*>(e);
			Command* command = commandEvent->command;
			bool executeCommandWhenAdding = commandEvent->execute;

			int nrOfCommandsBeforeAdd = m_commander->getNrOfCommands();
			//--------------------------------------------------------------------------------------
			// Update Command history
			//--------------------------------------------------------------------------------------
			bool addCommandSucceeded = true;
			if(executeCommandWhenAdding)
			{
				addCommandSucceeded = m_commander->tryToAddCommandToHistoryAndExecute(command);
			}
			else
			{
				addCommandSucceeded = m_commander->tryToAddCommandToHistory(command);
			}

			//--------------------------------------------------------------------------------------
			// Update GUI
			//--------------------------------------------------------------------------------------
			if(addCommandSucceeded)
			{
				int nrOfCommandsAfterAdd = m_commander->getNrOfCommands();
				int GUI_Index = Converter::convertBetweenCommandHistoryIndexAndGUIListIndex(m_commander->getCurrentCommandIndex(), nrOfCommandsAfterAdd);

				if(nrOfCommandsAfterAdd <= nrOfCommandsBeforeAdd)
				{
					int nrOfCommandToBeRemovedFromGUI = nrOfCommandsBeforeAdd - nrOfCommandsAfterAdd;
					SEND_EVENT(&Event_RemoveCommandsFromCommandHistoryGUI(GUI_Index, nrOfCommandToBeRemovedFromGUI+1));
				}
				SEND_EVENT(&Event_AddCommandToCommandHistoryGUI(command));
				SEND_EVENT(&Event_SetSelectedCommandGUI(GUI_Index));
			}
			else
			{
				MESSAGEBOX("Failed to add command to the command history. Make sure the command pointer is initialized before trying to add it.");
			}
			break;
		}
	case EVENT_TRACK_TO_COMMAND_HISTORY_INDEX:
		{
			Event_TrackToCommandHistoryIndex* commandHistoryIndexEvent = static_cast<Event_TrackToCommandHistoryIndex*>(e);
			int index = commandHistoryIndexEvent->indexOfCommand;
			if(!m_commander->tryToJumpInCommandHistory(index))
			{
				MESSAGEBOX("Failed to jump in the command history.");
			}
			break;
		}
	case EVENT_GET_COMMANDER_INFO:
		{
			Event_GetCommanderInfo* commanderInfoEvent = static_cast<Event_GetCommanderInfo*>(e);
			commanderInfoEvent->indexOfCurrentCommand = m_commander->getCurrentCommandIndex();
			commanderInfoEvent->nrOfCommands = m_commander->getNrOfCommands();
			break;
		}
	}
}