#include "stdafx.h"
#include "Manager_Commands.h"

#include <Core/World.h> //SETTINGS
#include <Core/Commander.h>
#include <Core/CommanderSpy.h>
#include <Core/Command_ChangeBackBufferColor.h>
#include <Core/Command_SkyBox.h>
#include <Core/Events.h>
#include "ui_MainWindow.h"
#include "Manager_Docks.h"
#include "Window.h"

void Manager_Commands::init()
{
	SUBSCRIBE_TO_EVENT(this, EVENT_STORE_COMMAND);
	SUBSCRIBE_TO_EVENT(this, EVENT_STORE_COMMANDS_AS_SINGLE_COMMAND_HISTORY_GUI_ENTRY);
	SUBSCRIBE_TO_EVENT(this, EVENT_TRACK_TO_COMMAND_HISTORY_INDEX);
	SUBSCRIBE_TO_EVENT(this, EVENT_GET_COMMANDER_INFO);
	SUBSCRIBE_TO_EVENT(this, EVENT_NEW_PROJECT);
	SUBSCRIBE_TO_EVENT(this, EVENT_SKYBOX_CHANGED);
	m_window = Window::instance();
	m_ui = m_window->ui();

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
	connect(a, SIGNAL(triggered()), this, SLOT(loadProjectFileDialog()));

	// Recent
	a = m_ui->actionRecent;
	a->setStatusTip(tr("Open recent project"));
	connect(a, SIGNAL(triggered()), this, SLOT(loadRecentCommandHistory()));

	// Save
	a = m_ui->actionSave;
	/*path = iconPath + "Menu/save";
	a->setIcon(QIcon(path.c_str())); */
	//a->setShortcuts(QKeySequence::Save);
	a->setStatusTip(tr("Save project"));
	connect(a, SIGNAL(triggered()), this, SLOT(quicksaveCommandHistory()));

	// Save as
	a = m_ui->actionSave_As;
	a->setStatusTip(tr("Save project to..."));
	connect(a, SIGNAL(triggered()), this, SLOT(saveProjectFileDialog()));

	
	// Undo
	a = m_ui->actionUndo;
	/*path = iconPath + "Menu/undo";
	a->setIcon(QIcon(path.c_str())); */
	a->setShortcuts(QKeySequence::Undo); 
	a->setStatusTip(tr("Undo last editing action"));
	connect(a, SIGNAL(triggered()), this, SLOT(undoLatestCommand()));

	// Redo
	a = m_ui->actionRedo;
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
	createTestButton("#6699e6", mapper);
	createTestButton("#00d423", mapper);
	createTestButton("#f30000", mapper);
	
	createTestButton("#fff", mapper);
	createTestButton("#000", mapper);

	// Toggle button
	{
		std::string path = "";
		std::string icon_name = "Skybox";
		path = path + ICON_PATH + "Options/" + icon_name;

		QAction* a = new QAction(QIcon(path.c_str()), icon_name.c_str(), m_window);
		m_action_skybox = a;
		a->setCheckable(true);
		a->setChecked(SETTINGS()->showSkybox());
		a->setToolTip("Toggle skybox");
		m_toolbar_commands->addAction(a);

		connect(a, SIGNAL(triggered(bool)), this, SLOT(enableSkybox(bool)));
	}
}

bool Manager_Commands::storeCommandInCommandHistory(Command* command, bool execute)
{
	bool addCommandSucceeded = true;
	if(execute)
	{
		addCommandSucceeded = m_commander->tryToAddCommandToHistoryAndExecute(command);
	}
	else
	{
		addCommandSucceeded = m_commander->tryToAddCommandToHistory(command);
	}
	return addCommandSucceeded;
}

void Manager_Commands::updateCommandHistoryGUI(Command* command, bool hiddenInGUIList, int GUI_MergeNumber, int nrOfCommandsBeforeAdd)
{
	int nrOfCommandsAfterAdd = m_commander->getNrOfCommands();
	int GUI_Index = Converter::convertBetweenCommandHistoryIndexAndGUIListIndex(m_commander->getCurrentCommandIndex(), nrOfCommandsAfterAdd);

	if(nrOfCommandsAfterAdd <= nrOfCommandsBeforeAdd) // If history overwrite took place: remove command representations from GUI.
	{
		int nrOfCommandToBeRemovedFromGUI = nrOfCommandsBeforeAdd - nrOfCommandsAfterAdd;
		SEND_EVENT(&Event_RemoveCommandsFromCommandHistoryGUI(GUI_Index, nrOfCommandToBeRemovedFromGUI+1));
	}
	SEND_EVENT(&Event_AddCommandToCommandHistoryGUI(command, hiddenInGUIList, GUI_MergeNumber));
}

bool Manager_Commands::jumpInCommandHistory(int commandHistoryIndex)
{
	if(!m_commander->tryToJumpInCommandHistory(commandHistoryIndex))
	{
		//QSound sound("Windows Ding.wav");
		//sound.play();

		static QSound sound("Windows Ding.wav");
		if(sound.isFinished()) // Still does not work as intended (2013-05-22 22.06) when holding down CTRL+Z or CTRL+Y
		{
			sound.play();
		}
		return false;
	}
	return true;
}

void Manager_Commands::updateCurrentCommandGUI()
{
	int GUI_Index = Converter::convertBetweenCommandHistoryIndexAndGUIListIndex(m_commander->getCurrentCommandIndex(), m_commander->getNrOfCommands());
	SEND_EVENT(&Event_SetSelectedCommandGUI(GUI_Index));
}

void Manager_Commands::loadCommandHistory(std::string path)
{
	if(m_commander->tryToLoadCommandHistory(path))
	{
		m_lastValidProjectPath = path;

		std::vector<Command*>* commands = m_commanderSpy->getCommands();
		int nrOfCommands = commands->size();
		for(int i=0;i<nrOfCommands;i++)
		{
			Command* command = commands->at(i);
			SEND_EVENT(&Event_AddCommandToCommandHistoryGUI(command, false)); // Add all commands to GUI //check false, enable hidden commands to be hidden, 2013-05-14 19.56
		}
		updateCurrentCommandGUI();

		// Uncomment to print command history information to debug file
		//m_commander->printCommandHistory("UltimateCoffe_CommandHistory_DEBUG_file_from_project_load.txt");
	}
	else
	{
		MESSAGEBOX("Failed to load project. Please contact Folke Peterson-Berger. Tell him that 'Princess Adelaide has the whooping cough'.");
	}
}

void Manager_Commands::saveCommandHistory(std::string path)
{
	if(!m_commander->tryToSaveCommandHistory(path))
	{
		MESSAGEBOX("Failed to save project.");
	}
	else
	{
		m_lastValidProjectPath = path;

		// Uncomment to print command history information to debug file
		//m_commander->printCommandHistory("UltimateCoffe_CommandHistory_DEBUG_file_from_project_save_as.txt");
	}
}

void Manager_Commands::setBackBufferColor(QString p_str_color)
{
	QColor color = (p_str_color);
	Command_ChangeBackBufferColor* command0 = new Command_ChangeBackBufferColor();
	command0->setDoColor(color.red()/255.0f, color.green()/255.0f, color.blue()/255.0f);
	command0->setUndoColor(SETTINGS()->backBufferColor.x, SETTINGS()->backBufferColor.y, SETTINGS()->backBufferColor.z);
	SEND_EVENT(&Event_StoreCommandInCommandHistory(command0, true));
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
	Event_GetNextOrPreviousVisibleCommandRowInCommandHistory returnValue(true, m_commander->getCurrentCommandIndex());
	SEND_EVENT(&returnValue);
	int rowNr = returnValue.row;
	
	// Special case: '-1' mean that the current command is outside of history. Do not jump to -1 (since that is the current command index already).
	// Refer to the special case regarding "m_commandHistoryListWidget->currentRow()"
	if(rowNr == -1)
	{
		rowNr = 0;
	}
	if(jumpInCommandHistory(rowNr))
	{
		updateCurrentCommandGUI();
	}
}

void Manager_Commands::undoLatestCommand()
{
	Event_GetNextOrPreviousVisibleCommandRowInCommandHistory returnValue(false, m_commander->getCurrentCommandIndex());
	SEND_EVENT(&returnValue);
	if(jumpInCommandHistory(returnValue.row))
	{
		updateCurrentCommandGUI();
	}
}

void Manager_Commands::quicksaveCommandHistory()
{
	if(m_lastValidProjectPath != "")
	{
		saveCommandHistory(m_lastValidProjectPath);
	}
	else
	{
		saveProjectFileDialog();
	}
}

void Manager_Commands::saveProjectFileDialog()
{
	// Opens standard Windows "save file" dialog
	QString fileName = QFileDialog::getSaveFileName(m_window, tr("Save Ultimate Coffee Project"), "UltimateCoffeeProject", tr("Ultimate Coffee Project (*.uc)"));

	// If the user clicks "Save"
	if(!fileName.isEmpty())
	{
		std::string path = fileName.toLocal8Bit();
		saveCommandHistory(path);
	}
}

void Manager_Commands::loadProjectFileDialog()
{
	// Opens standard Windows "open file" dialog
	QString fileName = QFileDialog::getOpenFileName(m_window, tr("Open Ultimate Coffee Project"), "UltimateCoffeeProject.uc", tr("Ultimate Coffee Project (*.uc)"));

	// If the user clicks "Open"
	if(!fileName.isEmpty())
	{
		SEND_EVENT(&Event_RemoveCommandsFromCommandHistoryGUI(0, m_commander->getNrOfCommands())); // Clear command history GUI list

		std::string path = fileName.toLocal8Bit();
		loadCommandHistory(path);
	}
}

void Manager_Commands::createTestButton( QString color, QSignalMapper* mapper )
{
	QAction* a = new QAction(m_window->createIcon(&QColor(color)), color, m_window);
	m_toolbar_commands->addAction(a);
	connect(a, SIGNAL(triggered()), mapper, SLOT(map()));
	mapper->setMapping(a, color);
}

void Manager_Commands::onEvent(Event* e)
{
	EventType type = e->type();
	switch (type)
	{
	case EVENT_SKYBOX_CHANGED:
		{
			m_action_skybox->setChecked(SETTINGS()->showSkybox());
		}
		break;
	case EVENT_STORE_COMMAND: // Add a command, sent in an event, to the commander. It might also be executed.
		{
			Event_StoreCommandInCommandHistory* commandEvent = static_cast<Event_StoreCommandInCommandHistory*>(e);
			Command* command = commandEvent->command;
			bool execute = commandEvent->execute;
			bool setAsCurrentInGUI = commandEvent->setAsCurrentInGUI;

			int nrOfCommandsBeforeAdd = m_commander->getNrOfCommands();
			if(!storeCommandInCommandHistory(command, execute))
			{
				MESSAGEBOX("Failed to add command to the command history. Make sure the command pointer is initialized before trying to add command to command history.");
			}
			updateCommandHistoryGUI(command, false, 0, nrOfCommandsBeforeAdd);
			if(setAsCurrentInGUI)
			{
				updateCurrentCommandGUI();
			}
			break;
		}
	case EVENT_STORE_COMMANDS_AS_SINGLE_COMMAND_HISTORY_GUI_ENTRY:
		{
			Event_StoreCommandsAsSingleEntryInCommandHistoryGUI* multipleCommandsEvent = static_cast<Event_StoreCommandsAsSingleEntryInCommandHistoryGUI*>(e);
			std::vector<Command*>* commands = multipleCommandsEvent->commands;
			bool execute = multipleCommandsEvent->execute;
			
			int nrOfCommands = commands->size();
			for(int i=0;i<nrOfCommands;i++)
			{
				int mergeNumber = 0;
				bool hidden = true;
				Command* command = commands->at(i);
				if(i==nrOfCommands-1)
				{
					hidden = false;
					mergeNumber = nrOfCommands;
				}
				int nrOfCommandsBeforeAdd = m_commander->getNrOfCommands();
				if(!storeCommandInCommandHistory(command, execute))
				{
					MESSAGEBOX("Failed to add command to the command history. Make sure the command pointer is initialized before trying to add command to command history.");
				}
				updateCommandHistoryGUI(command, hidden, mergeNumber, nrOfCommandsBeforeAdd);
			}
			updateCurrentCommandGUI();
			break;
		}
	case EVENT_TRACK_TO_COMMAND_HISTORY_INDEX:
		{
			Event_TrackToCommandHistoryIndex* commandHistoryIndexEvent = static_cast<Event_TrackToCommandHistoryIndex*>(e);
			int index = commandHistoryIndexEvent->indexOfCommand;

			jumpInCommandHistory(index);
			break;
		}
	case EVENT_GET_COMMANDER_INFO:
		{
			Event_GetCommanderInfo* commanderInfoEvent = static_cast<Event_GetCommanderInfo*>(e);
			commanderInfoEvent->indexOfCurrentCommand = m_commander->getCurrentCommandIndex();
			commanderInfoEvent->nrOfCommands = m_commander->getNrOfCommands();
			break;
		}
	case EVENT_NEW_PROJECT:
		{
			SEND_EVENT(&Event_RemoveCommandsFromCommandHistoryGUI(0, m_commander->getNrOfCommands())); // Clear command history GUI list
			m_commander->reset();
			break;
		}
	}
}

void Manager_Commands::enableSkybox( bool state )
{
	Command_SkyBox* command_SkyBox = new Command_SkyBox();
	command_SkyBox->setShowSkyBox(state);
	SEND_EVENT(&Event_StoreCommandInCommandHistory(command_SkyBox, true));
}

void Manager_Commands::loadRecentCommandHistory()
{
	//also refer to "loadProjectFileDialog"
	SEND_EVENT(&Event_RemoveCommandsFromCommandHistoryGUI(0, m_commander->getNrOfCommands())); // Clear command history GUI list

	std::string path = "./recent.uc";
	loadCommandHistory(path);
}
