#include "stdafx.h"
#include "Manager_Docks.h"
#include "Window.h"
#include "ui_MainWindow.h"

#include <Core/DataMapper.h>
#include <Core/Data.h>
#include <Core/Events.h>
#include <Core/Enums.h>
#include <Core/World.h>
#include <Core/Command_ChangeBackBufferColor.h>
#include <Core/Command_TranslateSceneEntity.h>
#include <Core/Command_RotateSceneEntity.h>
#include <Core/Command_ScaleSceneEntity.h>
#include <Core/Command_SkyBox.h>
#include <Core/Command_CreateEntity.h>

Manager_Docks::~Manager_Docks()
{
}

void Manager_Docks::init()
{
	SUBSCRIBE_TO_EVENT(this, EVENT_ADD_TO_COMMAND_HISTORY_GUI);
	SUBSCRIBE_TO_EVENT(this, EVENT_SET_SELECTED_COMMAND_GUI);
	SUBSCRIBE_TO_EVENT(this, EVENT_REMOVE_ALL_COMMANDS_FROM_CURRENT_ROW_IN_COMMAND_HISTORY_GUI);
	SUBSCRIBE_TO_EVENT(this, EVENT_ADD_ROOT_COMMAND_TO_COMMAND_HISTORY_GUI);
	SUBSCRIBE_TO_EVENT(this, EVENT_GET_COMMAND_HISTORY_GUI_FILTER);
	SUBSCRIBE_TO_EVENT(this, EVENT_INCREMENT_OR_DECREMENT_CURRENT_ROW_IN_COMMAND_HISTORY_GUI);
	SUBSCRIBE_TO_EVENT(this, EVENT_PLAY_SOUND_DING);
	
	m_commandHistoryListWidget = nullptr;
	m_window = Window::instance();
	m_menu = m_window->ui()->menuWindow;
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

	if(ok)
		ok = m_window->restoreGeometry(geo_data);
	if(ok)
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
	QAction* a;

	// Fullscreen
	a = createAction("Fullscreen");
	a->setCheckable(true);
	a->setShortcut(QKeySequence("F1"));
	connect(a, SIGNAL(toggled(bool)), m_window, SLOT(setFullscreen(bool)));

	// Maximize scene
	a = createAction("Maximize Scene");
	a->setCheckable(true);
	a->setShortcut(QKeySequence("Ctrl+G"));
	connect(a, SIGNAL(toggled(bool)), this, SLOT(setMaximizeScene(bool)));

	// Add dock action
	a = new QAction("Create Dock", m_window);
	a->setShortcut(QKeySequence("Ctrl+Shift+T"));
	connect(a, SIGNAL(triggered()), this, SLOT(createDockWidget()));
	m_menu->addAction(a);
	connect(m_window->ui()->actionReset_Layout, SIGNAL(triggered()), this, SLOT(resetLayout()));
	m_window->ui()->actionReset_Layout->setShortcut(QKeySequence("Ctrl+Shift+R"));
	connect(m_window->ui()->actionSave_Layout, SIGNAL(triggered()), this, SLOT(saveLayout()));
	connect(m_window->ui()->actionLoad_Layout, SIGNAL(triggered()), this, SLOT(loadLayout()));
	m_menu->addSeparator();


	QDockWidget* dock;

	// Scene
	dock = createDock("Scene", Qt::LeftDockWidgetArea);
	dock->setWidget(m_window->renderWidget());
	m_scene = dock;

	// Inspector
	dock = createDock("Inspector", Qt::RightDockWidgetArea);

	// Command History
	dock = createDock("History", Qt::LeftDockWidgetArea);
	m_commandHistoryListWidget = new ListWidgetWithoutKeyboardInput(dock);
	connectCommandHistoryListWidget(true);
	//connect(m_commandHistoryListWidget, SIGNAL(itemPressed(QListWidgetItem*)), this, SLOT(commandHistoryItemPressed(QListWidgetItem*)));
	dock->setWidget(m_commandHistoryListWidget);

	// Item Browser
	dock = createDock("Item Browser", Qt::LeftDockWidgetArea);
	dock->setWindowTitle("Item Browser");
	m_itemBrowser = new ItemBrowser(dock);
	dock->setWidget(m_itemBrowser);

	// Hierarchy
	dock = createDock("Hierarchy", Qt::RightDockWidgetArea);
	QTreeView* tree = new Hierarchy(m_window);
	m_hierarchy_tree = tree;
	tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
	tree->setAlternatingRowColors(true);
	tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_hierarchy_model = new QStandardItemModel(0, 1, this);
	m_hierarchy_model->setHorizontalHeaderItem(0, new QStandardItem("Entity ID"));
	tree->setModel(m_hierarchy_model);
	dock->setWidget(tree);
	connect(tree, SIGNAL(clicked( const QModelIndex &)), this, SLOT(selectEntity(const QModelIndex &)));
	connect(tree, SIGNAL(entered( const QModelIndex &)), this, SLOT(selectEntity(const QModelIndex &)));
	connect(tree, SIGNAL(doubleClicked( const QModelIndex &)), this, SLOT(focusOnEntity(const QModelIndex &)));


	// Coffee
	dock = createDock("Coffee", Qt::RightDockWidgetArea);
	QPlainTextEdit* textEdit = new QPlainTextEdit();
	textEdit->setReadOnly(true);
	textEdit->setPlainText(
		"\n"
		"  INITIALIZING\n"
		"\n"
		"    ~-_  Steamin'\n"
		"     _-~    Hot\n"
		"   c|_|   COFFEE\n"
		"");
	dock->setWidget(textEdit);

	// Tool
	{
		dock = createDock("Tool", Qt::RightDockWidgetArea);
		QWidget* w = new ToolPanel(dock);
		dock->setWidget(w);
	}

	// Console
	dock = createDock("Console", Qt::LeftDockWidgetArea);
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

	// Give focus to RenderWidget
	m_window->renderWidget()->setFocus();
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

ISystem* Manager_Docks::getAsSystem()
{
	return new System_Editor(this);
}

void Manager_Docks::onEvent(Event* e)
{
	EventType type = e->type();
	switch (type) 
	{
	case EVENT_ADD_TO_COMMAND_HISTORY_GUI: // Add command or commands to the command history list in the GUI
		{
			Event_AddToCommandHistoryGUI* commandEvent = static_cast<Event_AddToCommandHistoryGUI*>(e);
			std::vector<Command*>* commands = commandEvent->commands;
			bool displayAsSingleCommandHistoryEntry = commandEvent->displayAsSingleCommandHistoryEntry;
			int indexToBundledWithCommandHistoryGUIListEntry = commandEvent->indexToBundledWithCommandHistoryGUIListEntry;

			int nrOfCommandToBeAdded = commands->size();
			for(int i=0;i<nrOfCommandToBeAdded;i++)
			{
				std::string commandText = "UNKNOWN COMMAND";
				std::string appendToCommandText = "";
				if(displayAsSingleCommandHistoryEntry && nrOfCommandToBeAdded > 1)
				{
					appendToCommandText = " (" + Converter::IntToStr(nrOfCommandToBeAdded) +")";
				}

				QIcon commandIcon;
				Command* command = commands->at(i);
				Enum::CommandType type = command->getType();
				switch(type)
				{
				case Enum::CommandType::CHANGEBACKBUFFERCOLOR:
					{
						commandText = "Background color";
						Command_ChangeBackBufferColor* changeBackBufferColorEvent = static_cast<Command_ChangeBackBufferColor*>(command);
				
						float x = changeBackBufferColorEvent->getDoColorX() * 255;
						float y = changeBackBufferColorEvent->getDoColorY() * 255;
						float z = changeBackBufferColorEvent->getDoColorZ() * 255;

						QColor color(x,y,z);
						QPixmap pixmap(16, 16);
						pixmap.fill(color);
						commandIcon.addPixmap(pixmap);
						break;
					}
				case Enum::CommandType::TRANSLATE_SCENE_ENTITY:
					{
						commandText = "Translation";
						Command_TranslateSceneEntity* translateSceneEntityEvent = static_cast<Command_TranslateSceneEntity*>(command);

						std::string iconPath = ICON_PATH;
						iconPath += "Tools/translate";
						commandIcon.addFile(iconPath.c_str());
						break;
					}
				case Enum::CommandType::ROTATE_SCENE_ENTITY:
					{
						commandText = "Rotation";
						Command_RotateSceneEntity* translateSceneEntityEvent = static_cast<Command_RotateSceneEntity*>(command);

						std::string iconPath = ICON_PATH;
						iconPath += "Tools/rotate";
						commandIcon.addFile(iconPath.c_str());
						break;
					}
				case Enum::CommandType::SCALE_SCENE_ENTITY:
					{
						commandText = "Scaling";
						Command_ScaleSceneEntity* translateSceneEntityEvent = static_cast<Command_ScaleSceneEntity*>(command);
					
						std::string iconPath = ICON_PATH;
						iconPath += "Tools/scale";
						commandIcon.addFile(iconPath.c_str());
						break;
					}
				case Enum::CommandType::SKYBOX:
					{
						Command_SkyBox* skyboxCommand = static_cast<Command_SkyBox*>(command);

						// If backbuffer change	
						if(skyboxCommand->dataStruct_.skyBoxIndex == 0)
						{
							float x = skyboxCommand->dataStruct_.doColor.x * 255;
							float y = skyboxCommand->dataStruct_.doColor.y * 255;
							float z = skyboxCommand->dataStruct_.doColor.z * 255;

							QColor color(x,y,z);
							QPixmap pixmap(16, 16);
							pixmap.fill(color);
							commandIcon.addPixmap(pixmap);
							commandText = "Background color";
						}
						// Skybox
						else
						{
							commandText = "Skybox changed";
							std::string iconPath = ICON_PATH;
							
							iconPath += "Options/Skybox";
							int skyboxIndex = skyboxCommand->getSkyBoxIndex();

							if(skyboxIndex > 0 && skyboxIndex < 10)
							{
								std::string number = Converter::IntToStr(skyboxIndex);
								iconPath += number;
							}
							else
							{
								MESSAGEBOX("Missing skybox icon. void Manager_Docks::onEvent(Event* e).");
							}
							
							commandIcon.addFile(iconPath.c_str());
						}

						break;
					}
				case Enum::CommandType::CREATE_ENTITY:
					{
						commandText = "Entity creation";

						std::string iconPath = ICON_PATH;
						iconPath += "Tools/Geometry";
						commandIcon.addFile(iconPath.c_str());

						break;
					}
				case Enum::CommandType::REMOVE_ENTITY:
					{
						commandText = "Entity removal";

						std::string iconPath = ICON_PATH;
						iconPath += "Tools/Remove";
						commandIcon.addFile(iconPath.c_str());

						break;
					}
				default:
					{
						std::string iconPath = ICON_PATH;
						iconPath += "Tools/Coffee";
						commandIcon.addFile(iconPath.c_str());
						break;
					}
				}
				commandText += appendToCommandText;
				QString commandtextAsQString = commandText.c_str();

				// Special case: give the added command history entry the command history index that is the current index in the command history
				if(indexToBundledWithCommandHistoryGUIListEntry == -2)
				{
					// Get info from "Manager_Commands"
					Event_GetCommandHistoryInfo returnValue;
					SEND_EVENT(&returnValue);
					if(returnValue.indexOfCurrentCommand < -1) // Invalid command index. The "Event_GetCommandHistoryInfo" event might have got lost somewhere.
					{
						MESSAGEBOX("Error when adding command(s) to the command list GUI. Added command(s) may not work as intended.");
					}
					else
					{
						indexToBundledWithCommandHistoryGUIListEntry = returnValue.indexOfCurrentCommand;
					}
				}
				else
				{
					if(i != 0)
					{
						indexToBundledWithCommandHistoryGUIListEntry++;
					}
				}
				addItemToCommandHistoryListWidget(commandIcon, commandtextAsQString, indexToBundledWithCommandHistoryGUIListEntry);

				if(displayAsSingleCommandHistoryEntry)
				{
					break; // Exit loop. Only add one command to the GUI.
				}
			}
		}
		break;
	case EVENT_SET_SELECTED_COMMAND_GUI:
		{
			Event_SetSelectedCommandGUI* selectionEvent = static_cast<Event_SetSelectedCommandGUI*>(e);
			int commandHistoryIndex = selectionEvent->commandHistoryIndex;

			int listItemIndex = findListItemIndexFromCommandHistoryIndex(commandHistoryIndex);
			m_commandHistoryListWidget->setCurrentRow(listItemIndex);
		}
		break;
	case EVENT_REMOVE_ALL_COMMANDS_FROM_CURRENT_ROW_IN_COMMAND_HISTORY_GUI:
		{
			Event_RemoveAllCommandsAfterCurrentRowFromCommandHistoryGUI* removeCommandFromGUIEvent = static_cast<Event_RemoveAllCommandsAfterCurrentRowFromCommandHistoryGUI*>(e);
			int removeAll = removeCommandFromGUIEvent->removeAllCommands;

			connectCommandHistoryListWidget(false);
			if(removeAll)
			{
				m_commandHistoryListWidget->clear(); // Remove all commands from the GUI list at once
			}
			else // Remove all command after current row
			{
				int nextRowAfterCurrentRow = m_commandHistoryListWidget->currentRow() + 1;
				int nrOfCommandsInTheGUIList = m_commandHistoryListWidget->count();
				int nrOfCommandsToBeRemovedFromTheGUIList = nrOfCommandsInTheGUIList - nextRowAfterCurrentRow;
				int i = 0;
				while(i < nrOfCommandsToBeRemovedFromTheGUIList)
				{
					delete m_commandHistoryListWidget->takeItem(nextRowAfterCurrentRow); // "takeItem" affects current selected item of the widget, creating an unwanted SIGNAL. Therefore "connectCommandHistoryListWidget(false);" is used above, to prevent the SIGNAL from being handled.
					i++;
				}
			}
			connectCommandHistoryListWidget(true);
		}
		break;
	case EVENT_ADD_ROOT_COMMAND_TO_COMMAND_HISTORY_GUI:
		{
			if(m_commandHistoryListWidget->count() == 0)
			{
				QIcon icon;
				addItemToCommandHistoryListWidget(icon, "Start", -1);
			}
			else
			{
				MESSAGEBOX("Trying to add 'Start' to command history GUI list when the list is not empty. This operation is not supported.")
			}
		}
		break;
	case EVENT_GET_COMMAND_HISTORY_GUI_FILTER:
		{
			Event_GetCommandHistoryGUIFilter* getGUIFilterEvent = static_cast<Event_GetCommandHistoryGUIFilter*>(e);
			
			std::vector<bool>* GUIFilter = new std::vector<bool>;
			int nrOfCommands = m_commandHistoryListWidget->count();
			for(int i=1;i<nrOfCommands;i++) // i = 1, ignores ROOT_COMMAND
			{
				QListWidgetItem* item = m_commandHistoryListWidget->item(i);
				if(item->isHidden())
				{
					GUIFilter->push_back(true);
				}
				else
				{
					GUIFilter->push_back(false);
				}
			}
			getGUIFilterEvent->GUIFilter = GUIFilter;
		}
		break;
	case EVENT_INCREMENT_OR_DECREMENT_CURRENT_ROW_IN_COMMAND_HISTORY_GUI:
		{
			Event_IncrementOrDecrementCurrentRowInCommandHistoryGUI* incrementOrDecrement = static_cast<Event_IncrementOrDecrementCurrentRowInCommandHistoryGUI*>(e);
			bool increment = incrementOrDecrement->if_true_increment_if_false_decrement;

			int nrOfRows = m_commandHistoryListWidget->count();
			int currentRow = m_commandHistoryListWidget->currentRow();
			if(increment)
			{
				if(currentRow+1 < nrOfRows)
				{
					m_commandHistoryListWidget->setCurrentRow(currentRow+1);
				}
				else
				{
					playDingSound();
				}
			}
			else
			{
				if(currentRow-1 > -1)
				{
					m_commandHistoryListWidget->setCurrentRow(currentRow-1);
				}
				else
				{
					playDingSound();
				}
			}
		}
		break;
	case EVENT_PLAY_SOUND_DING:
		{
			playDingSound();
		}
		break;
	default:
		break;
	}
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

void Manager_Docks::connectCommandHistoryListWidget(bool connect_if_true_otherwise_disconnect)
{
	if(connect_if_true_otherwise_disconnect)
	{
		connect(m_commandHistoryListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(currentCommandHistoryIndexChanged(int)));
	}
	else
	{
		disconnect(m_commandHistoryListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(currentCommandHistoryIndexChanged(int)));
	}
}

void Manager_Docks::addItemToCommandHistoryListWidget(const QIcon& icon, const QString& text, int index)
{
	// Add custom list item (with index retrievable on row change, refer to "Manager_Docks::currentCommandHistoryIndexChanged")
	ListItemWithIndex* indexedItem = new ListItemWithIndex(icon, text, index);
	m_commandHistoryListWidget->addItem(indexedItem);
}

int Manager_Docks::findListItemIndexFromCommandHistoryIndex(int commandHistoryIndex)
{
	int nrOfCommands = m_commandHistoryListWidget->count();
	int foundCommandHistoryIndexFromIndexItem = -2;
	for(int i=0;i<nrOfCommands;i++)
	{
		QListWidgetItem* item = m_commandHistoryListWidget->item(i);
		int currentCommandHistoryIndexFromIndexItem = getIndexFromItemWithIndex(item);
		if(currentCommandHistoryIndexFromIndexItem == commandHistoryIndex)
		{
			foundCommandHistoryIndexFromIndexItem = i;
			break;
		}
	}
	return foundCommandHistoryIndexFromIndexItem;
}

int Manager_Docks::getIndexFromItemWithIndex(QListWidgetItem* item)
{
	//QVariant data = item->data(0);
	//int commandHistoryIndex = data.toInt();
	ListItemWithIndex* indexItem = static_cast<ListItemWithIndex*>(item);
	int commandHistoryIndex = indexItem->getIndex();
	return commandHistoryIndex;
}

void Manager_Docks::playDingSound()
{
	static QSound sound("Windows Ding.wav");
	if(sound.isFinished()) // Still does not work as intended (2013-05-22 22.06) when holding down CTRL+Z or CTRL+Y
	{
		sound.play();
	}
}

class EntityItem : public QStandardItem
{
public:
	int m_entityId;

public:
	EntityItem(int id ,const QString& text) : QStandardItem(text)
	{
		m_entityId = id;
	}

public:
	int entityId()
	{
		return m_entityId;
	}
};

class QStandardItem_Category : public QStandardItem
{
public:
	std::vector<int> empty_spots;
};

class QStandardItem_Entity : public QStandardItem
{
public:
	int entityId;
};

void Manager_Docks::setupHierarchy()
{
	// Create categories
	for(int i=0; i<Enum::Entity_End; i++)
	{
		QStandardItem* category = m_hierarchy_model->item(i);
		if(!category)
		{
			m_hierarchy_model->setItem(i, new QStandardItem_Category());

			// Hide category, until used
			m_hierarchy_tree->setRowHidden(i, m_hierarchy_tree->rootIndex(), true);
		}
	}
}



void Manager_Docks::update()
{
	// Add entities to list
	DataMapper<Data::Created> map_created;
	while(map_created.hasNext())
	{
		Entity* e = map_created.nextEntity();
		e->removeData<Data::Created>();
		int type = e->type();
		int entityId = e->id();

		// Make room for item category
		QStandardItem* category = m_hierarchy_model->item(type);
		if(category)
		{
			// Update item
			QStandardItem_Entity* item = new QStandardItem_Entity();
			item->entityId = e->id();
			item->setText(e->name().c_str());
			e->hierarchyRow = category->rowCount();
			category->setChild(e->hierarchyRow, item);

			// Update category
			std::string typeName = e->typeName();
			m_hierarchy_tree->setRowHidden(type, m_hierarchy_tree->rootIndex(), false);
			typeName += " [" + Converter::IntToStr(category->rowCount()) + "]";
			category->setText(typeName.c_str());
		}
	}
	
	// Remove entries from list
// 	DataMapper<Data::Deleted> map_removed;
// 	while(map_removed.hasNext())
// 	{
// 		Entity* e = map_removed.nextEntity();
// 		e->removeData<Data::Deleted>();
// 		int type = e->type();
// 		int entityId = e->id();
// 
// 		// Find item and delete it
// 		QStandardItem* category = m_hierarchy_model->item(type);
// 		if(category)
// 		{
// 			// Update item
// 			QStandardItem* item = new QStandardItem(e->name().c_str());
// 			e->hierarchyRow = category->rowCount();
// 			category->setChild(e->hierarchyRow, item);
// 
// 			// Update category
// 			std::string typeName = e->typeName();
// 			m_hierarchy_tree->setRowHidden(type, m_hierarchy_tree->rootIndex(), false);
// 			typeName += " [" + Converter::IntToStr(category->rowCount()) + "]";
// 			category->setText(typeName.c_str());
// 		}
// 
// 		QStandardItem* item = m_hierarchy_model->item(entityId);
// 		if(item)
// 		{
// 			item->setEnabled(false);
// 			item->setSelectable(false);
// 			m_hierarchy_tree->setRowHidden(entityId, m_hierarchy_tree->rootIndex(), true);
// 		}
// 	}
}

void Manager_Docks::currentCommandHistoryIndexChanged(int currentRow)
{
	QListWidgetItem* currentItem = m_commandHistoryListWidget->item(currentRow);
	int commandHistoryIndexStoredInItemAtCurrentRow = getIndexFromItemWithIndex(currentItem);
	
	Event_GetCommandHistoryInfo commandHistoryInfo; // Retrieve information from command history
	SEND_EVENT(&commandHistoryInfo); // The event is assumed to have correct values below
	int indexOfCurrentCommand = commandHistoryInfo.indexOfCurrentCommand;
	
	if(indexOfCurrentCommand != commandHistoryIndexStoredInItemAtCurrentRow) // Only jump when not jumping to the index that is already current
	{
		SEND_EVENT(&Event_JumpToCommandHistoryIndex(commandHistoryIndexStoredInItemAtCurrentRow));
	}
}

void Manager_Docks::selectEntity( const QModelIndex& index )
{
//	QStandardItem_Entity* clicked 

	DataMapper<Data::Selected> map_selected;

	// Remove previous selection
	Data::Selected::clearSelection();

	// Add new selection
	QList<QModelIndex> index_list = m_hierarchy_tree->selectionModel()->selectedRows();
	if(index_list.count() == 0)
		Data::Selected::lastSelected.invalidate();
	foreach(QModelIndex index, index_list)
	{
		int entityId = index.row();
		Entity* e = Entity::findEntity(entityId);
		e->addData(Data::Selected());
	}

	// If clicked was selected (not deselected with CTRL click)
	// add as LastClicked
	Entity* clickedEntity = Entity::findEntity(index.row());
	if(clickedEntity->fetchData<Data::Selected>())
		Data::Selected::select(clickedEntity);
	else
		Data::Selected::findLastSelected();

	// Inform about selection
	SEND_EVENT(&Event(EVENT_ENTITY_SELECTION));
}

void Manager_Docks::focusOnEntity( const QModelIndex& index )
{
	// Fetch camera
	Entity* entity_camera = CAMERA_ENTITY().asEntity();


	// Allow camera to focus on the entity double-clicked on
	Entity* clickedEntity = Entity::findEntity(index.row());
	Data::ZoomTo d_zoomTo;
	d_zoomTo.target = clickedEntity->toPointer();
	entity_camera->addData(d_zoomTo);
	
	DEBUGPRINT("Focus on Entity: " + Converter::IntToStr(clickedEntity->id()));
}


void System_Editor::update()
{
	m_editor->update();
}

ItemBrowser::ItemBrowser( QWidget* p_parent ) : QWidget(p_parent)
{
	SUBSCRIBE_TO_EVENT(this, EVENT_REFRESH_SPLITTER);
	SUBSCRIBE_TO_EVENT(this, EVENT_PREVIEW_ITEMS);
	POST_DELAYED_EVENT(new Event(EVENT_REFRESH_SPLITTER), 0.0f);

	setObjectName("Item Browser");

	QWidget* window = Window::instance();
	m_tree = new QListWidget(window);
	m_tree->setObjectName("Item Tree");
	m_grid = new QListWidget(window);
	m_grid->setObjectName("Item Grid");
	m_grid->setIconSize(QSize(65, 65));
	m_grid->setViewMode(QListView::IconMode);
	m_grid->setLayoutMode(QListWidget::LayoutMode::Batched);
	m_grid->setResizeMode(QListWidget::ResizeMode::Adjust);
	m_grid->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_grid->setMovement(QListView::Static);
	m_grid->setWordWrap(true);

	QVBoxLayout* l = new QVBoxLayout(this);
	setLayout(l);

	// Create splitter
	m_splitter = new QSplitter(window);
	m_tree->setObjectName("Item Splitter");
	l->addWidget(m_splitter);
	m_splitter->addWidget(m_tree);
	m_splitter->addWidget(m_grid);

	// Load tree
	initTree();
	
	// Select first folder
	QListWidgetItem* item = m_tree->item(0);
	if(item)
	{
		item->setSelected(true);
		loadGrid(0);
	}

	// Enable mouse click
	connect(m_tree, SIGNAL(currentRowChanged(int)), this,  SLOT(loadGrid(int)));
	connect(m_grid, SIGNAL(itemClicked(QListWidgetItem*)), this,  SLOT(selectEntity(QListWidgetItem*)));
}

void ItemBrowser::initTree()
{
	// Open 
	QString path;
	path = path + THUMBNAIL_PATH;
	
	// Load directory
	QDir dir(path);
	QStringList filters;
	dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

	// Build from directory
	QFileInfoList list = dir.entryInfoList();
	foreach(QFileInfo i, list)
	{
		QString filename = i.fileName();
		//DEBUGPRINT(filename.toStdString());

		QListWidgetItem* item = new QListWidgetItem(filename);
		m_tree->addItem(item);
	}
}

void ItemBrowser::loadGrid( QListWidgetItem* item )
{
	// Path
	QString path;
	path = path + THUMBNAIL_PATH + item->text();
	std::string str_path = path.toStdString();

	// Load items
	QFile textFile(path + "/_items.txt");
	if(textFile.open(QIODevice::ReadOnly))
	{
		QRegExp rx_tab("(\\t)");
		QRegExp rx_comma("(\\,)");

		QTextStream textStream(&textFile);
		while(!textStream.atEnd())
		{
			// Read line
			QString line = textStream.readLine();

			// Ignore comments and empty lines
			if(!(line[0] == '#' || line.size() == 0))
			{
				QStringList list = line.split(rx_tab);
				QString qstr_mesh = list[0];
				QString qstr_name = list[1];
				QString qstr_color = list[2];
				list = qstr_color.split(rx_comma);
				QString c_r = list[0];
				QString c_g = list[1];
				QString c_b = list[2];

				// Read mesh
				int meshId = Converter::StrToInt(qstr_mesh.toStdString());
				Enum::Mesh mesh = static_cast<Enum::Mesh>(meshId);

				// Read color
				QColor c(Converter::StrToInt(c_r.toStdString()), Converter::StrToInt(c_g.toStdString()), Converter::StrToInt(c_b.toStdString()));
				Color color = Vector3(c.redF(), c.greenF(), c.blueF());

				// Create item
				QIcon icon(path + "/" + qstr_name);
				Item_Prefab* item = new Item_Prefab(icon, qstr_name);
				item->color = color;
				item->mesh = mesh;
				m_grid->addItem(item);
			}
		}
	}
	else
	{
		// Open 
		QString path;
		path = path + THUMBNAIL_PATH + "/" + item->text();
		QDir dir(path);
		QStringList filters;
		filters << "*.png" << "*.jpg";
		dir.setNameFilters(filters);
		dir.setFilter(QDir::Files);

		QFileInfoList list = dir.entryInfoList();
		foreach(QFileInfo i, list)
		{
			QString filename = i.baseName();

			QIcon icon(path + "/" + filename);
			Item_Prefab* item = new Item_Prefab(icon, filename);
			m_grid->addItem(item);
		}
	}
}

void ItemBrowser::loadGrid( int row )
{
	m_grid->clear();
	loadGrid(m_tree->item(row));
}

void ItemBrowser::onEvent( Event* e )
{
	EventType type = e->type();
	switch (type) 
	{
	case EVENT_REFRESH_SPLITTER: //Add command to the command history list in the GUI
		moveHandle();
		break;
	case EVENT_PREVIEW_ITEMS:
		 {
			 std::vector<Command*> command_list;

			 // HACK: Compensate for change in tools due to gridselection
			 int toolTyp = SETTINGS()->selectedTool();

			 Vector3 offset = Math::randomVector(-0.1f,0.1f);
			 offset = offset + CAMERA_ENTITY().asEntity()->fetchData<Data::Transform>()->position;
			 offset.z += 15.0f; 
			 int row = 0;
			 int col = 0;

			 // Create startup items
			 Entity* e;
			 for(int i=0; i<m_grid->count(); i++)
			 {
				 selectEntity(m_grid->item(i));
				 e = FACTORY_ENTITY()->createEntity(Enum::Entity_Mesh);
				 Data::Transform* d_transform = e->fetchData<Data::Transform>();
				 d_transform->position.x = offset.x + col;
				 d_transform->position.y = offset.y - row;
				 d_transform->position.z = offset.z;

				 
				 col++;
				 if(col > 5)
				 {
					 col = 0;
					 row++;
				 }
				 
				 // Save command
				 command_list.push_back(new Command_CreateEntity(e, true));
			 }

			 // Save to history
			 if(command_list.size() > 0)
				 SEND_EVENT(&Event_AddToCommandHistory(&command_list, false));

			 SETTINGS()->setSelectedTool(toolTyp);
		 }
		 break;
	default:
		break;
	}
}

void ItemBrowser::selectEntity( QListWidgetItem* item )
{
	// Switch to Geometry Tool
	if(SETTINGS()->selectedTool() != Enum::Tool_Geometry)
		SETTINGS()->setSelectedTool(Enum::Tool_Geometry);

	// Select corresponding Entity
	Item_Prefab* i = static_cast<Item_Prefab*>(item);
	SETTINGS()->choosenEntity.color = i->color;
	SETTINGS()->choosenEntity.mesh = i->mesh;
}

void Hierarchy::keyPressEvent( QKeyEvent *e )
{
	QCoreApplication::sendEvent(parentWidget(), e);
}

void Hierarchy::keyReleaseEvent( QKeyEvent *e )
{
	QCoreApplication::sendEvent(parentWidget(), e);
}

//void ToolPanel::valueChanged(double d)
//{
//	int test = 3;
//}

void ToolPanel::setXTranslationOfSelectedEntities(double p_transX)
{
	if(spinboxValueSetBecauseOfSelectionOrTransformation == false)
	{
		std::vector<Command*> translationCommands;
		DataMapper<Data::Selected> map_selected;
		Entity* e;
		unsigned int i = 0;

		while(map_selected.hasNext())
		{
			e = map_selected.nextEntity();

			Data::Transform* trans = e->fetchData<Data::Transform>();
			Command_TranslateSceneEntity *command = new Command_TranslateSceneEntity(e->id());
			command->setDoTranslation(p_transX, trans->position.y, trans->position.z);
			command->setUndoTranslation(trans->position.x, trans->position.y, trans->position.z);
			translationCommands.push_back(command);
		
			trans->position.x = p_transX;

			++i;
		}

		if(translationCommands.size() > 0)
			SEND_EVENT(&Event_AddToCommandHistory(&translationCommands, false));
	}
}

void ToolPanel::setYTranslationOfSelectedEntities(double p_transY)
{
	if(spinboxValueSetBecauseOfSelectionOrTransformation == false)
	{
		std::vector<Command*> translationCommands;
		DataMapper<Data::Selected> map_selected;
		Entity* e;
		unsigned int i = 0;

		while(map_selected.hasNext())
		{
			e = map_selected.nextEntity();

			Data::Transform* trans = e->fetchData<Data::Transform>();
			Command_TranslateSceneEntity *command = new Command_TranslateSceneEntity(e->id());
			command->setDoTranslation(trans->position.x, p_transY, trans->position.z);
			command->setUndoTranslation(trans->position.x, trans->position.y, trans->position.z);
			translationCommands.push_back(command);
		
			trans->position.y = p_transY;

			++i;
		}

		if(translationCommands.size() > 0)
			SEND_EVENT(&Event_AddToCommandHistory(&translationCommands, false));
	}
}

void ToolPanel::setZTranslationOfSelectedEntities(double p_transZ)
{
	if(spinboxValueSetBecauseOfSelectionOrTransformation == false)
	{
		std::vector<Command*> translationCommands;
		DataMapper<Data::Selected> map_selected;
		Entity* e;
		unsigned int i = 0;

		while(map_selected.hasNext())
		{
			e = map_selected.nextEntity();

			Data::Transform* trans = e->fetchData<Data::Transform>();
			Command_TranslateSceneEntity *command = new Command_TranslateSceneEntity(e->id());
			command->setDoTranslation(trans->position.x, trans->position.y, p_transZ);
			command->setUndoTranslation(trans->position.x, trans->position.y, trans->position.z);
			translationCommands.push_back(command);
		
			trans->position.z = p_transZ;

			++i;
		}

		if(translationCommands.size() > 0)
			SEND_EVENT(&Event_AddToCommandHistory(&translationCommands, false));
	}
}

void ToolPanel::setXScalingOfSelectedEntities(double p_xScale)
{
	if(spinboxValueSetBecauseOfSelectionOrTransformation == false)
	{
		std::vector<Command*> scalingCommands;
		DataMapper<Data::Selected> map_selected;
		Entity* e;
		unsigned int i = 0;

		bool atLeastOneObjectWasSelected = false;
		while(map_selected.hasNext())
		{
			atLeastOneObjectWasSelected = true;

			e = map_selected.nextEntity();

			Data::Transform* trans = e->fetchData<Data::Transform>();
			Command_ScaleSceneEntity *command = new Command_ScaleSceneEntity(e->id());
			command->setDoScale(p_xScale, trans->scale.y, trans->scale.z);
			command->setUndoScale(trans->scale.x, trans->scale.y, trans->scale.z);
			scalingCommands.push_back(command);
		
			trans->scale.x = p_xScale;

			++i;
		}

		if(scalingCommands.size() > 0)
			SEND_EVENT(&Event_AddToCommandHistory(&scalingCommands, false));
	}
}

void ToolPanel::setYScalingOfSelectedEntities(double p_yScale)
{
	if(spinboxValueSetBecauseOfSelectionOrTransformation == false)
	{
		std::vector<Command*> scalingCommands;
		DataMapper<Data::Selected> map_selected;
		Entity* e;
		unsigned int i = 0;

		bool atLeastOneObjectWasSelected = false;
		while(map_selected.hasNext())
		{
			atLeastOneObjectWasSelected = true;

			e = map_selected.nextEntity();

			Data::Transform* trans = e->fetchData<Data::Transform>();
			Command_ScaleSceneEntity *command = new Command_ScaleSceneEntity(e->id());
			command->setDoScale(trans->scale.x, p_yScale, trans->scale.z);
			command->setUndoScale(trans->scale.x, trans->scale.y, trans->scale.z);
			scalingCommands.push_back(command);
		
			trans->scale.y = p_yScale;

			++i;
		}

		if(atLeastOneObjectWasSelected)
			SEND_EVENT(&Event_AddToCommandHistory(&scalingCommands, false));
	}
}

void ToolPanel::setZScalingOfSelectedEntities(double p_zScale)
{
	if(spinboxValueSetBecauseOfSelectionOrTransformation == false)
	{
		std::vector<Command*> scalingCommands;
		DataMapper<Data::Selected> map_selected;
		Entity* e;
		unsigned int i = 0;

		bool atLeastOneObjectWasSelected = false;
		while(map_selected.hasNext())
		{
			atLeastOneObjectWasSelected = true;

			e = map_selected.nextEntity();

			Data::Transform* trans = e->fetchData<Data::Transform>();
			Command_ScaleSceneEntity *command = new Command_ScaleSceneEntity(e->id());
			command->setDoScale(trans->scale.x, trans->scale.y, p_zScale);
			command->setUndoScale(trans->scale.x, trans->scale.y, trans->scale.z);
			scalingCommands.push_back(command);
		
			trans->scale.z = p_zScale;

			++i;
		}

		if(atLeastOneObjectWasSelected)
			SEND_EVENT(&Event_AddToCommandHistory(&scalingCommands, false));
	}
}

void ToolPanel::setXRotationOfSelectedEntities(double p_rotX)
{
	if(spinboxValueSetBecauseOfSelectionOrTransformation == false)
	{
		std::vector<Command*> rotationCommands;
		DataMapper<Data::Selected> map_selected;
		Entity* e;
		unsigned int i = 0;

		bool atLeastOneObjectWasSelected = false;
		while(map_selected.hasNext())
		{
			atLeastOneObjectWasSelected = true;

			e = map_selected.nextEntity();

			Data::Transform* trans = e->fetchData<Data::Transform>();
			Command_RotateSceneEntity *command = new Command_RotateSceneEntity(e->id());

			XMVECTOR quat = trans->rotation;

			float xAngle = rotationXSpinBox->value();
			float yAngle = rotationYSpinBox->value();
			float zAngle = rotationZSpinBox->value();

			XMVECTOR rotQuat = XMQuaternionRotationRollPitchYaw(p_rotX * (Math::Pi / 180), yAngle * (Math::Pi / 180), zAngle * (Math::Pi / 180));

			command->setDoRotQuat(rotQuat.m128_f32[0], rotQuat.m128_f32[1], rotQuat.m128_f32[2], rotQuat.m128_f32[3]);
			command->setUndoRotQuat(quat.m128_f32[0], quat.m128_f32[1], quat.m128_f32[2], quat.m128_f32[3]);
			rotationCommands.push_back(command);
		
			trans->rotation = rotQuat;

			++i;
		}

		if(atLeastOneObjectWasSelected)
			SEND_EVENT(&Event_AddToCommandHistory(&rotationCommands, false));
	}
}

void ToolPanel::setYRotationOfSelectedEntities(double p_rotY)
{
	if(spinboxValueSetBecauseOfSelectionOrTransformation == false)
	{
		std::vector<Command*> rotationCommands;
		DataMapper<Data::Selected> map_selected;
		Entity* e;
		unsigned int i = 0;

		bool atLeastOneObjectWasSelected = false;
		while(map_selected.hasNext())
		{
			atLeastOneObjectWasSelected = true;

			e = map_selected.nextEntity();

			Data::Transform* trans = e->fetchData<Data::Transform>();
			Command_RotateSceneEntity *command = new Command_RotateSceneEntity(e->id());

			XMVECTOR quat = trans->rotation;

			float xAngle = rotationXSpinBox->value();
			float yAngle = rotationYSpinBox->value();
			float zAngle = rotationZSpinBox->value();

			XMVECTOR rotQuat = XMQuaternionRotationRollPitchYaw(xAngle * (Math::Pi / 180), p_rotY * (Math::Pi / 180), zAngle * (Math::Pi / 180));

			command->setDoRotQuat(rotQuat.m128_f32[0], rotQuat.m128_f32[1], rotQuat.m128_f32[2], rotQuat.m128_f32[3]);
			command->setUndoRotQuat(quat.m128_f32[0], quat.m128_f32[1], quat.m128_f32[2], quat.m128_f32[3]);
			rotationCommands.push_back(command);
		
			trans->rotation = rotQuat;

			++i;
		}

		if(atLeastOneObjectWasSelected)
			SEND_EVENT(&Event_AddToCommandHistory(&rotationCommands, false));
	}
}

void ToolPanel::setZRotationOfSelectedEntities(double p_rotZ)
{
	if(spinboxValueSetBecauseOfSelectionOrTransformation == false)
	{
		std::vector<Command*> rotationCommands;
		DataMapper<Data::Selected> map_selected;
		Entity* e;
		unsigned int i = 0;

		bool atLeastOneObjectWasSelected = false;
		while(map_selected.hasNext())
		{
			atLeastOneObjectWasSelected = true;

			e = map_selected.nextEntity();

			Data::Transform* trans = e->fetchData<Data::Transform>();
			Command_RotateSceneEntity *command = new Command_RotateSceneEntity(e->id());

			XMVECTOR quat = trans->rotation;

			float xAngle = rotationXSpinBox->value();
			float yAngle = rotationYSpinBox->value();
			float zAngle = rotationZSpinBox->value();

			XMVECTOR rotQuat = XMQuaternionRotationRollPitchYaw(xAngle * (Math::Pi / 180), yAngle * (Math::Pi / 180), p_rotZ * (Math::Pi / 180));

			command->setDoRotQuat(rotQuat.m128_f32[0], rotQuat.m128_f32[1], rotQuat.m128_f32[2], rotQuat.m128_f32[3]);
			command->setUndoRotQuat(quat.m128_f32[0], quat.m128_f32[1], quat.m128_f32[2], quat.m128_f32[3]);
			rotationCommands.push_back(command);
		
			trans->rotation = rotQuat;

			++i;
		}

		if(atLeastOneObjectWasSelected)
			SEND_EVENT(&Event_AddToCommandHistory(&rotationCommands, false));
	}
}

ToolPanel::ToolPanel( QWidget* parent ) : QWidget(parent)
{
	SUBSCRIBE_TO_EVENT(this, EVENT_TRANSLATE_SCENE_ENTITY);
	SUBSCRIBE_TO_EVENT(this, EVENT_ROTATE_SCENE_ENTITY);
	SUBSCRIBE_TO_EVENT(this, EVENT_SCALE_SCENE_ENTITY);
	SUBSCRIBE_TO_EVENT(this, EVENT_ENTITY_SELECTION);

	spinboxValueSetBecauseOfSelectionOrTransformation = false;

	m_window = Window::instance();
	m_colorDialog = new QColorDialog(this);
	connect(m_colorDialog, SIGNAL(currentColorChanged(const QColor&)), this, SLOT(setColor(const QColor &)));

	QLayout* l = new QVBoxLayout(this);
	setLayout(l);
	l->setMargin(0);
	QScrollArea* scroll = new QScrollArea(this);
	scroll->setFrameShape(QFrame::NoFrame);
	l->addWidget(scroll);
	scroll->setWidgetResizable(true);

	QWidget* w;
	w = new QWidget(scroll);
	scroll->setWidget(w);
	QLayout* vl = new QVBoxLayout(w);
	w->setLayout(vl);
	const float SPINBOX_WIDH = 0.5f;
	{
		QLabel* l;
		QDoubleSpinBox *dsb;

		vl->addWidget(new QLabel("Position"));
		QLayout* hl = new QHBoxLayout(w);
		vl->addItem(hl);

		l = new QLabel("  X  ", w);
		dsb = new QDoubleSpinBox(w);
		dsb->setRange(-1000.0f, 1000.0f);
		dsb->setWrapping(1);
		dsb->setSingleStep(1);
		dsb->setKeyboardTracking(false);
		dsb->setMaximumWidth(dsb->width()*SPINBOX_WIDH);
		l->setMaximumSize(l->sizeHint());
		hl->addWidget(l);
		hl->addWidget(dsb);
		connect(dsb, SIGNAL(valueChanged(double)), this, SLOT(setXTranslationOfSelectedEntities(double)));
		translationXSpinBox = dsb;

		l = new QLabel("  Y  ", w);
		dsb = new QDoubleSpinBox(w);
		dsb->setRange(-1000.0f, 1000.0f);
		dsb->setWrapping(1);
		dsb->setSingleStep(1);
		dsb->setKeyboardTracking(false);
		dsb->setMaximumWidth(dsb->width()*SPINBOX_WIDH);
		l->setMaximumSize(l->sizeHint());
		hl->addWidget(l);
		hl->addWidget(dsb);
		connect(dsb, SIGNAL(valueChanged(double)), this, SLOT(setYTranslationOfSelectedEntities(double)));
		translationYSpinBox = dsb;

		l = new QLabel("  Z  ", w);
		dsb = new QDoubleSpinBox(w);
		dsb->setRange(-1000.0f, 1000.0f);
		dsb->setWrapping(1);
		dsb->setSingleStep(1);
		dsb->setKeyboardTracking(false);
		dsb->setMaximumWidth(dsb->width()*SPINBOX_WIDH);
		l->setMaximumSize(l->sizeHint());
		hl->addWidget(l);
		hl->addWidget(dsb);
		connect(dsb, SIGNAL(valueChanged(double)), this, SLOT(setZTranslationOfSelectedEntities(double)));
		translationZSpinBox = dsb;
	}
	{
		QLabel* l;
		QDoubleSpinBox *dsb;

		vl->addWidget(new QLabel("Rotation"));
		QLayout* hl = new QHBoxLayout(w);
		vl->addItem(hl);

		l = new QLabel("  X  ", w);
		dsb = new QDoubleSpinBox(w);
		dsb->setRange(-1000.0f, 1000.0f);
		dsb->setWrapping(1);
		dsb->setSingleStep(1);
		dsb->setKeyboardTracking(false);
		dsb->setMaximumWidth(dsb->width()*SPINBOX_WIDH);
		l->setMaximumSize(l->sizeHint());
		hl->addWidget(l);
		hl->addWidget(dsb);
		connect(dsb, SIGNAL(valueChanged(double)), this, SLOT(setXRotationOfSelectedEntities(double)));
		rotationXSpinBox = dsb;

		l = new QLabel("  Y  ", w);
		dsb = new QDoubleSpinBox(w);
		dsb->setRange(-1000.0f, 1000.0f);
		dsb->setWrapping(1);
		dsb->setSingleStep(1);
		dsb->setMaximumWidth(dsb->width()*SPINBOX_WIDH);
		l->setMaximumSize(l->sizeHint());
		hl->addWidget(l);
		hl->addWidget(dsb);
		connect(dsb, SIGNAL(valueChanged(double)), this, SLOT(setYRotationOfSelectedEntities(double)));
		rotationYSpinBox = dsb;

		l = new QLabel("  Z  ", w);
		dsb = new QDoubleSpinBox(w);
		dsb->setRange(-1000.0f, 1000.0f);
		dsb->setWrapping(1);
		dsb->setSingleStep(1);
		dsb->setMaximumWidth(dsb->width()*SPINBOX_WIDH);
		l->setMaximumSize(l->sizeHint());
		hl->addWidget(l);
		hl->addWidget(dsb);
		connect(dsb, SIGNAL(valueChanged(double)), this, SLOT(setZRotationOfSelectedEntities(double)));
		rotationZSpinBox = dsb;
	}
	{
		QLabel* l;
		QDoubleSpinBox *dsb;

		vl->addWidget(new QLabel("Scale"));
		QLayout* hl = new QHBoxLayout(w);
		vl->addItem(hl);

		l = new QLabel("  X  ", w);
		dsb = new QDoubleSpinBox(w);
		dsb->setRange(0.0f, 1000.0f);
		dsb->setWrapping(1);
		dsb->setSingleStep(1);
		dsb->setKeyboardTracking(false);
		dsb->setMaximumWidth(dsb->width()*SPINBOX_WIDH);
		l->setMaximumSize(l->sizeHint());
		hl->addWidget(l);
		hl->addWidget(dsb);
		connect(dsb, SIGNAL(valueChanged(double)), this, SLOT(setXScalingOfSelectedEntities(double)));
		scalingXSpinBox = dsb;

		l = new QLabel("  Y  ", w);
		dsb = new QDoubleSpinBox(w);
		dsb->setRange(0.0f, 1000.0f);
		dsb->setWrapping(1);
		dsb->setSingleStep(1);
		dsb->setKeyboardTracking(false);
		dsb->setMaximumWidth(dsb->width()*SPINBOX_WIDH);
		l->setMaximumSize(l->sizeHint());
		hl->addWidget(l);
		hl->addWidget(dsb);
		connect(dsb, SIGNAL(valueChanged(double)), this, SLOT(setYScalingOfSelectedEntities(double)));
		scalingYSpinBox = dsb;

		l = new QLabel("  Z  ", w);
		dsb = new QDoubleSpinBox(w);
		dsb->setRange(0.0f, 1000.0f);
		dsb->setWrapping(1);
		dsb->setSingleStep(1);
		dsb->setKeyboardTracking(false);
		dsb->setMaximumWidth(dsb->width()*SPINBOX_WIDH);
		l->setMaximumSize(l->sizeHint());
		hl->addWidget(l);
		hl->addWidget(dsb);
		connect(dsb, SIGNAL(valueChanged(double)), this, SLOT(setZScalingOfSelectedEntities(double)));
		scalingZSpinBox = dsb;
	}
	{
		QLabel* l;

		QPushButton* button;
		vl->addWidget(new QLabel("Color"));
		QLayout* hl = new QHBoxLayout(w);
		vl->addItem(hl);

		l = new QLabel("      ", w);
		//l->setMaximumSize(l->sizeHint());
		hl->addWidget(l);
		QPixmap pixmap(100, 20);
		pixmap.fill(QColor());
		QIcon icon(pixmap);
		l = new QLabel("hello", w);
		m_colorIcon = l;
		l->setPixmap(pixmap);
		l->setMaximumSize(l->sizeHint());
		l->setStyleSheet("border: 1px solid #f5f5f5;");
		hl->addWidget(l);

		l = new QLabel(" ", w);
		l->setMaximumSize(l->sizeHint());
		hl->addWidget(l);

		button = new QPushButton("...", w);
		QSize size = button->sizeHint();
		button->setMaximumSize(QSize(25,20));
		connect(button, SIGNAL(clicked()), this, SLOT(pickColor()));
		//setColor(QColor(3, 137, 255));
		hl->addWidget(button);

		l = new QLabel("          ", w);
		l->setMaximumSize(l->sizeHint());
		hl->addWidget(l);
	}
	vl->addItem(m_window->createSpacer(Qt::Vertical));
}

void ToolPanel::onEvent(Event *p_event)
{
	EventType type = p_event->type();

	static Vector2 currentScreenCoords;

	DataMapper<Data::Selected> map_selected;
	bool thereIsAtLeastOneSelectedEntity = map_selected.hasNext();

	switch(type) 
	{
	case EVENT_TRANSLATE_SCENE_ENTITY:
		{
			//if(Data::Selected::lastSelected.isValid())
			//{
			//	spinboxValueSetBecauseOfSelectionOrTransformation = true;

			//	translationXSpinBox->setValue(Data::Selected::lastSelected->toPointer()->fetchData<Data::Transform>()->position.x);
			//	translationYSpinBox->setValue(Data::Selected::lastSelected->toPointer()->fetchData<Data::Transform>()->position.y);
			//	translationZSpinBox->setValue(Data::Selected::lastSelected->toPointer()->fetchData<Data::Transform>()->position.z);

			//	spinboxValueSetBecauseOfSelectionOrTransformation = false;
			//}
			break;
		}
	case EVENT_ROTATE_SCENE_ENTITY:
		{
			//if(Data::Selected::lastSelected.isValid())
			//{
			//	spinboxValueSetBecauseOfSelectionOrTransformation = true;

			//	spinboxValueSetBecauseOfSelectionOrTransformation = false;
			//}
			break;
		}
	case EVENT_SCALE_SCENE_ENTITY:
		{
			//if(Data::Selected::lastSelected.isValid())
			//{
			//	spinboxValueSetBecauseOfSelectionOrTransformation = true;

			//	scalingXSpinBox->setValue(Data::Selected::lastSelected->toPointer()->fetchData<Data::Transform>()->scale.x);
			//	scalingYSpinBox->setValue(Data::Selected::lastSelected->toPointer()->fetchData<Data::Transform>()->scale.y);
			//	scalingZSpinBox->setValue(Data::Selected::lastSelected->toPointer()->fetchData<Data::Transform>()->scale.z);

			//	spinboxValueSetBecauseOfSelectionOrTransformation = false;
			//}
			break;
		}
	case EVENT_ENTITY_SELECTION:
		{
			if(Data::Selected::lastSelected.isValid())
			{
				spinboxValueSetBecauseOfSelectionOrTransformation = true;

				Data::Transform *trans = Data::Selected::lastSelected->toPointer()->fetchData<Data::Transform>();

				if(trans)
				{
					translationXSpinBox->setValue(trans->position.x);
					translationYSpinBox->setValue(trans->position.y);
					translationZSpinBox->setValue(trans->position.z);

					XMVECTOR quat = trans->rotation;

					float xAngle = atan2(	(2 * (quat.m128_f32[0] * quat.m128_f32[1] + quat.m128_f32[2] * quat.m128_f32[3])),	(1 - 2 * (pow(quat.m128_f32[1], 2) + pow(quat.m128_f32[2], 2)))		);

					float yAngle =  asin(	 2 * (quat.m128_f32[0] * quat.m128_f32[2] - quat.m128_f32[3] * quat.m128_f32[1])	);

					float zAngle = atan2(	(2 * (quat.m128_f32[0] * quat.m128_f32[3] + quat.m128_f32[1] * quat.m128_f32[2]) ),	(1 - 2 * (pow(quat.m128_f32[2], 2) + pow(quat.m128_f32[3], 2)))		);

					rotationXSpinBox->setValue(xAngle * (Math::Pi / 180));
					rotationYSpinBox->setValue(yAngle * (Math::Pi / 180));
					rotationZSpinBox->setValue(zAngle * (Math::Pi / 180));

					scalingXSpinBox->setValue(trans->scale.x);
					scalingYSpinBox->setValue(trans->scale.y);
					scalingZSpinBox->setValue(trans->scale.z);

					spinboxValueSetBecauseOfSelectionOrTransformation = false;
				}

				// Set color icon
				Data::Render* d_render = Data::Selected::lastSelected->toPointer()->fetchData<Data::Render>();
				if(d_render)
				{
					Color c = d_render->mesh.color;
					QColor color;
					color.setRedF(c.x);
					color.setGreenF(c.y);
					color.setBlueF(c.z);
					QPixmap pixmap(100, 20);
					pixmap.fill(color);
					QIcon icon(pixmap);
					m_colorIcon->setPixmap(pixmap);
				}
			}
			break;
		}
	case EVENT_SET_TOOL:
		{
			break;
		}
	default:
		break;
	}
}

void ToolPanel::pickColor()
{
	QColor c;

	// Pick color from last selected entity
	EntityPointer e = Data::Selected::lastSelected;
	if(e.isValid())
	{
		Data::Render* d_render = e->fetchData<Data::Render>();
		if(d_render)
		{
			Color color = d_render->mesh.color;
			c.setRedF(color.x);
			c.setGreenF(color.y);
			c.setBlueF(color.z);
			m_colorDialog->setCurrentColor(c);
		}
	}
	
	m_colorDialog->show();
}

void ToolPanel::setColor( const QColor& color )
{
	DataMapper<Data::Selected> map_selected;
	while(map_selected.hasNext())
	{
		Entity* e = map_selected.nextEntity();
		Data::Render* d_render = e->fetchData<Data::Render>();
		if(d_render)
		{
			Color c;
			c.x = color.redF();
			c.y = color.greenF();
			c.z = color.blueF();
			d_render->mesh.color = c;
		}
	}

	// Set icon
	QPixmap pixmap(100, 20);
	pixmap.fill(color);
	QIcon icon(pixmap);
	m_colorIcon->setPixmap(pixmap);
}

ListItemWithIndex::ListItemWithIndex(const QIcon& icon, const QString& text, int index)
	: QListWidgetItem(icon, text)
{
	m_index = index;
}

int ListItemWithIndex::getIndex()
{
	return m_index;
}

Item_Prefab::Item_Prefab( QIcon icon, QString filname ) : QListWidgetItem(icon, filname)
{
	// Pick random mesh
	int meshId = Math::randomInt(0, Enum::Mesh_End-1);
	mesh = static_cast<Enum::Mesh>(meshId);

	// Pick random color
	color = Math::randomColor();
}
