#include "stdafx.h"
#include "Manager_Commands.h"

#include <Core/World.h> //SETTINGS
#include <Core/CommandHistory.h>
#include <Core/CommandHistorySpy.h>
#include <Core/Command_ChangeBackBufferColor.h>
#include <Core/Command_SkyBox.h>
#include <Core/Events.h>
#include "ui_MainWindow.h"
#include "Manager_Docks.h"
#include "Window.h"

void Manager_Commands::init()
{
	SUBSCRIBE_TO_EVENT(this, EVENT_ADD_TO_COMMAND_HISTORY);
	SUBSCRIBE_TO_EVENT(this, EVENT_JUMP_TO_COMMAND_HISTORY_INDEX);
	SUBSCRIBE_TO_EVENT(this, EVENT_GET_COMMAND_HISTORY_INFO);
	SUBSCRIBE_TO_EVENT(this, EVENT_NEW_PROJECT);
	SUBSCRIBE_TO_EVENT(this, EVENT_SKYBOX_CHANGED);
	m_window = Window::instance();
	m_ui = m_window->ui();

	projectedLoaded = false;

	// Init undo/redo system
	m_commandHistory = new CommandHistory();
	m_commandHistorySpy = new CommandHistorySpy(m_commandHistory);
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

	m_skyboxGroup = new QActionGroup(this);

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

	// Create skybox
	{
		std::string path = "";
		std::string icon_name = "Skybox1";
		path = path + ICON_PATH + "Options/" + icon_name;

		QAction* a = createTestButton("#000", mapper);
		a->setChecked(true);
		a->setIcon(QIcon(path.c_str()));
		a->setObjectName(icon_name.c_str());
		a->setToolTip("Select skybox 1");

		m_action_skybox = a;
	}
	{
		std::string path = "";
		std::string icon_name = "Skybox2";
		path = path + ICON_PATH + "Options/" + icon_name;

		QAction* a = createTestButton("#000", mapper);
		a->setIcon(QIcon(path.c_str()));
		a->setObjectName(icon_name.c_str());
		a->setToolTip("Select skybox 2");

		m_action_skybox2 = a;
	}
}

bool Manager_Commands::storeCommandsInCommandHistory(std::vector<Command*>* commands, bool execute)
{
	int nrOfCommands = commands->size();
	bool addCommandSucceeded = false;
	addCommandSucceeded = m_commandHistory->tryToAddCommands(commands, execute);
	return addCommandSucceeded;
}

void Manager_Commands::updateCommandHistoryGUI(std::vector<Command*>* commands, bool displayAsSingleCommandHistoryEntry)
{
	if(m_commandHistorySpy->getHistoryOverWriteTookPlaceWhenAddingCommands()) // If history overwrite took place: remove command representations from GUI.
	{
		SEND_EVENT(&Event_RemoveAllCommandsAfterCurrentRowFromCommandHistoryGUI());
	}
	SEND_EVENT(&Event_AddToCommandHistoryGUI(commands, displayAsSingleCommandHistoryEntry));
	updateCurrentCommandGUI();
}

bool Manager_Commands::jumpInCommandHistory(int commandHistoryIndex)
{
	if(!m_commandHistory->tryToJumpInCommandHistory(commandHistoryIndex))
	{
		SEND_EVENT(&Event(EVENT_PLAY_SOUND_DING));
		return false;
	}
	return true;
}

void Manager_Commands::updateCurrentCommandGUI()
{
	int commandHistoryIndex = m_commandHistorySpy->getIndexOfCurrentCommand();
	SEND_EVENT(&Event_SetSelectedCommandGUI(commandHistoryIndex));
}

void Manager_Commands::newProject()
{
	int nrOfCommands = m_commandHistorySpy->getNrOfCommands();
	clearCommandHistoryGUI();
	m_commandHistory->reset();
	SEND_EVENT(&Event(EVENT_ADD_ROOT_COMMAND_TO_COMMAND_HISTORY_GUI));
	updateCurrentCommandGUI();
}

void Manager_Commands::loadCommandHistory(std::string path)
{
	newProject();

	int bufferSize = tryToGetFileSize(path);
	char* bytes = new char[bufferSize];
	if(Converter::FileToBytes(path, bytes, bufferSize))
	{
		m_lastValidProjectPath = path;

		m_commandHistory->reset();

//--------------------------------------------------------------------------------------
// Load hack after rewrite of command history GUI handling 2013-05-29 04.11
//--------------------------------------------------------------------------------------
		if(m_commandHistory->tryToLoadFromSerializationByteFormat(bytes, bufferSize, false))
		{
			//std::string pathWithoutUCFileExtension = path.substr(0, path.size()-3);

			CommandHistory* newCommandHistory = new CommandHistory();
			CommandHistory* oldCommandHistory = m_commandHistory;
			CommandHistorySpy* newCommandHistorySpy = new CommandHistorySpy(newCommandHistory);
			CommandHistorySpy* oldCommandHistorySpy = m_commandHistorySpy;

			m_commandHistory = newCommandHistory;
			m_commandHistorySpy = newCommandHistorySpy;

			std::vector<Command*>* allCommands = oldCommandHistorySpy->getCommands();
			int currentCommandAfterLoad = oldCommandHistorySpy->getIndexOfCurrentCommand();

			int nrOfCommands = allCommands->size();
			for(int i=0;i<nrOfCommands;i++)
			{
				bool addCommandSucceeded = false;
				Command* command = allCommands->at(i);
				addCommandSucceeded = m_commandHistory->tryToAddCommand(command, true);
				if(!addCommandSucceeded)
				{
					MESSAGEBOX("Failed to load project");
				}
				if(m_commandHistorySpy->getHistoryOverWriteTookPlaceWhenAddingCommands()) // If history overwrite took place: remove command representations from GUI.
				{
					SEND_EVENT(&Event_RemoveAllCommandsAfterCurrentRowFromCommandHistoryGUI());
				}
				std::vector<Command*> guiCommands;
				guiCommands.push_back(command);
				SEND_EVENT(&Event_AddToCommandHistoryGUI(&guiCommands, false));
			}

			m_commandHistory->tryToJumpInCommandHistory(currentCommandAfterLoad);
			updateCurrentCommandGUI();

			oldCommandHistories.push_back(oldCommandHistory);
			//delete oldCommandHistory;
			delete oldCommandHistorySpy;
			projectedLoaded = true;
		}
		else
		{
			MESSAGEBOX("Failed to load project");
		}
	}
	else
	{
		MESSAGEBOX("Failed to open project file. Please contact Folke Peterson-Berger. Tell him that 'Princess Adelaide has the whooping cough'.");
	}
	delete [] bytes;
}

void Manager_Commands::saveCommandHistory(std::string path)
{
	int sizeOfBytes;
	char* bytes = m_commandHistory->receiveSerializedByteFormat(sizeOfBytes); // "sizeOfBytes" is sent by reference
	if(Converter::BytesToFile(bytes, sizeOfBytes, path))
	{
		m_lastValidProjectPath = path;

		//std::string pathWithoutUCFileExtension = path.substr(0, path.size()-3);
		//saveCommandHistoryGUIFilter(pathWithoutUCFileExtension + "_GUIFilter.ucg");
	}
	else
	{
		MESSAGEBOX("Failed to save project.");
	}
	delete[] bytes;
}

//void Manager_Commands::saveCommandHistoryGUIFilter(std::string path)
//{
//	Event_GetCommandHistoryGUIFilter returnValue;
//	SEND_EVENT(&returnValue);
//	std::vector<bool>* GUIFilter = returnValue.GUIFilter;
//
//	if(sizeof(bool) != sizeof(char))
//	{
//		MESSAGEBOX("Unexpected discrepancy. Failed to save GUI Filter.");
//		return;
//	}
//
//	int nrOfBools = GUIFilter->size();
//	char* bytes = new char[nrOfBools];
//	for(int i=0;i<nrOfBools;i++)
//	{
//		bytes[i] = GUIFilter->at(i);
//	}
//
//	int sizeOfBytes = sizeof(bool)*nrOfBools;
//	if(!Converter::BytesToFile(bytes, sizeOfBytes, path))
//	{
//		MESSAGEBOX("Failed to save command history GUI filter");
//	}
//	delete[] bytes;
//	delete returnValue.GUIFilter; // Handle this deallocation in some better way.
//}

//void Manager_Commands::loadCommandHistoryGUIFilter(std::string path)
//{
//	if(sizeof(bool) != sizeof(char))
//	{
//		MESSAGEBOX("Unexpected discrepancy. Failed to load GUI Filter.");
//		return;
//	}
//
//	std::vector<Command*>* allCommands = m_commandHistorySpy->getCommands();
//
//	std::vector<bool> GUIFilter;
//	int bufferSize = tryToGetFileSize(path);
//	char* bytes = new char[bufferSize];
//	int nrOfBools = bufferSize/sizeof(bool);
//	if(Converter::FileToBytes(path, bytes, bufferSize))
//	{
//		for(int i=0;i<nrOfBools;i++)
//		{
//			GUIFilter.push_back(bytes[i]);
//		}
//
//		std::vector<Command*> subSetOfAllCommands;
//		bool singleEntryInCommandHistoryMode = false;
//		int i = 0;
//		int counter = 0;
//		while(i < nrOfBools)
//		{
//			bool hidden = GUIFilter.at(i);
//			if(!hidden)
//			{
//				subSetOfAllCommands.push_back(allCommands->at(i));
//			}
//			else
//			{
//				SEND_EVENT(&Event_AddToCommandHistoryGUI(&subSetOfAllCommands, false));
//				subSetOfAllCommands.clear();
//
//				while(hidden)
//				{
//					subSetOfAllCommands.push_back(allCommands->at(i));
//					i++;
//					if(i >= nrOfBools)
//					{
//						break;
//					}
//					hidden = GUIFilter.at(i);
//				}
//				subSetOfAllCommands.push_back(allCommands->at(i));
//				SEND_EVENT(&Event_AddToCommandHistoryGUI(&subSetOfAllCommands, true));
//				subSetOfAllCommands.clear();
//			}
//			i++;
//		}
//		SEND_EVENT(&Event_AddToCommandHistoryGUI(&subSetOfAllCommands, false));
//	}
//	else
//	{
//		// If the file could not be found or read, add commands to GUI without GUI filter
//		SEND_EVENT(&Event_AddToCommandHistoryGUI(allCommands, false));
//	}
//	delete [] bytes;
//}

void Manager_Commands::clearCommandHistoryGUI()
{
	SEND_EVENT(&Event_RemoveAllCommandsAfterCurrentRowFromCommandHistoryGUI(true)); // true = clear command hsitory GUI list
}

int Manager_Commands::tryToGetFileSize(std::string path)
{
	int bufferSize = 1000000; // Standard size
	struct stat results;
	if(stat(path.c_str(), &results) == 0)
	{
		bufferSize = results.st_size; // size of file, if "struct stat" succeeded
	}
	return bufferSize;
}

void Manager_Commands::setBackBufferColor(QString p_str_color)
{
	QColor color = (p_str_color);


	Command_SkyBox* c = new Command_SkyBox();

	if(m_action_skybox->isChecked())
		c->dataStruct_.skyBoxIndex = 1;
	if(m_action_skybox2->isChecked())
		c->dataStruct_.skyBoxIndex = 2;

	c->dataStruct_.doColor = Vector3(color.redF(), color.greenF(), color.blueF());
	c->dataStruct_.undoColor = Vector3(SETTINGS()->backBufferColor.x, SETTINGS()->backBufferColor.y, SETTINGS()->backBufferColor.z); 

	SEND_EVENT(&Event_AddToCommandHistory(c, true));
}

void Manager_Commands::translateSceneEntity()
{

}

Manager_Commands::~Manager_Commands()
{
	for(int i=0;i<oldCommandHistories.size();i++)
	{
		delete oldCommandHistories.at(i);
	}
	if(!projectedLoaded)
	{
		delete m_commandHistory;
	}
	delete m_commandHistorySpy;
}

void Manager_Commands::redoLatestCommand()
{
	SEND_EVENT(&Event_IncrementOrDecrementCurrentRowInCommandHistoryGUI(true));
}

void Manager_Commands::undoLatestCommand()
{
	SEND_EVENT(&Event_IncrementOrDecrementCurrentRowInCommandHistoryGUI(false));
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
		std::string path = fileName.toLocal8Bit();
		loadCommandHistory(path);
	}
}

QAction* Manager_Commands::createTestButton( QString color, QSignalMapper* mapper )
{
	QAction* a = new QAction(m_window->createIcon(&QColor(color)), color, m_window);
	m_toolbar_commands->addAction(a);
	connect(a, SIGNAL(triggered()), mapper, SLOT(map()));
	mapper->setMapping(a, color);

	a->setCheckable(true);
	m_skyboxGroup->addAction(a);

	return a;
}

void Manager_Commands::onEvent(Event* e)
{
	EventType type = e->type();
	switch (type)
	{
	case EVENT_SKYBOX_CHANGED:
		{
			//m_action_skybox->setChecked(SETTINGS()->showSkybox());
		}
		break;
	case EVENT_ADD_TO_COMMAND_HISTORY: // Add a list of commands, sent in an event, to the command history.
		{
			Event_AddToCommandHistory* commandEvent = static_cast<Event_AddToCommandHistory*>(e);
			std::vector<Command*>* commands = commandEvent->commands;
			Command* command = commandEvent->command;
			bool execute = commandEvent->execute;
			bool displayAsSingleCommandHistoryEntry = commandEvent->displayAsSingleCommandHistoryEntry;

			// Special case for backward compatibility: if the "command" variable of the event is not NULL, the "commands" is assumed to be unused.
			// Create "commands" and add "command" to it.
			bool deallocateCommandList = false;
			if(command != NULL)
			{
				commands = new std::vector<Command*>;
				commands->push_back(command);
				deallocateCommandList = true;
			}

			if(!storeCommandsInCommandHistory(commands, execute))
			{
				MESSAGEBOX("Failed to add command to the command history. Make sure the command pointer is initialized before trying to add command to command history.");
			}
			updateCommandHistoryGUI(commands, displayAsSingleCommandHistoryEntry);
			if(deallocateCommandList)
			{
				delete commands;
			}
			break;
		}
	case EVENT_JUMP_TO_COMMAND_HISTORY_INDEX:
		{
			Event_JumpToCommandHistoryIndex* commandHistoryIndexEvent = static_cast<Event_JumpToCommandHistoryIndex*>(e);
			int index = commandHistoryIndexEvent->commandHistoryIndex;

			jumpInCommandHistory(index);
			break;
		}
	case EVENT_GET_COMMAND_HISTORY_INFO:
		{
			Event_GetCommandHistoryInfo* commandHistoryInfoEvent = static_cast<Event_GetCommandHistoryInfo*>(e);
			commandHistoryInfoEvent->indexOfCurrentCommand = m_commandHistorySpy->getIndexOfCurrentCommand();
			commandHistoryInfoEvent->nrOfCommands = m_commandHistorySpy->getNrOfCommands();
			break;
		}
	case EVENT_NEW_PROJECT:
		{
			newProject();
			break;
		}
	}
}

void Manager_Commands::enableSkybox( bool state )
{
}

void Manager_Commands::loadRecentCommandHistory()
{
	std::string path = "./recent.uc";
	loadCommandHistory(path);
}
