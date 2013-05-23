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

Manager_Docks::~Manager_Docks()
{
}

void Manager_Docks::init()
{
	SUBSCRIBE_TO_EVENT(this, EVENT_ADD_COMMAND_TO_COMMAND_HISTORY_GUI);
	SUBSCRIBE_TO_EVENT(this, EVENT_SET_SELECTED_COMMAND_GUI);
	SUBSCRIBE_TO_EVENT(this, EVENT_REMOVE_SPECIFIED_COMMANDS_FROM_COMMAND_HISTORY_GUI);
	SUBSCRIBE_TO_EVENT(this, EVENT_GET_NEXT_VISIBLE_COMMAND_ROW);
	SUBSCRIBE_TO_EVENT(this, EVENT_ADD_ROOT_COMMAND_TO_COMMAND_HISTORY_GUI);
	
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
	a->setShortcuts(QKeySequence::AddTab);
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
	m_commandHistoryListWidget = new QListWidget(dock);
	connectCommandHistoryWidget(true);
	dock->setWidget(m_commandHistoryListWidget);

	// Item Browser
	dock = createDock("Item Browser", Qt::LeftDockWidgetArea);
	dock->setWindowTitle("Item Browser (Not fully implemented, as of 2013-05-23)");
	m_itemBrowser = new ItemBrowser(dock);
	dock->setWidget(m_itemBrowser);

	// Hierarchy
	dock = createDock("Hierarchy", Qt::RightDockWidgetArea);
	QTreeView* tree = new QTreeView(m_window);
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
		QWidget* w = new QWidget(dock);
		dock->setWidget(w);
		QLayout* l = new QVBoxLayout(w);
		w->setLayout(l);
		l->setMargin(0);
		QScrollArea* scroll = new QScrollArea(w);
		scroll->setFrameShape(QFrame::NoFrame);
		l->addWidget(scroll);
		scroll->setWidgetResizable(true);
		w = new QWidget(scroll);
		scroll->setWidget(w);
		QLayout* vl = new QVBoxLayout(w);
		w->setLayout(vl);
		{
			QLabel* l;
			vl->addWidget(new QLabel("Position"));
			QLayout* hl = new QHBoxLayout(w);
			vl->addItem(hl);
			l = new QLabel("  X  ", w);
			l->setMaximumSize(l->sizeHint());
			hl->addWidget(l);
			hl->addWidget(new QDoubleSpinBox(w));
			l = new QLabel("  Y  ", w);
			l->setMaximumSize(l->sizeHint());
			hl->addWidget(l);
			hl->addWidget(new QDoubleSpinBox(w));
			l = new QLabel("  Z  ", w);
			l->setMaximumSize(l->sizeHint());
			hl->addWidget(l);
			hl->addWidget(new QDoubleSpinBox(w));
		}
		{
			QLabel* l;
			vl->addWidget(new QLabel("Rotation"));
			QLayout* hl = new QHBoxLayout(w);
			vl->addItem(hl);
			l = new QLabel("  X  ", w);
			l->setMaximumSize(l->sizeHint());
			hl->addWidget(l);
			hl->addWidget(new QDoubleSpinBox(w));
			l = new QLabel("  Y  ", w);
			l->setMaximumSize(l->sizeHint());
			hl->addWidget(l);
			hl->addWidget(new QDoubleSpinBox(w));
			l = new QLabel("  Z  ", w);
			l->setMaximumSize(l->sizeHint());
			hl->addWidget(l);
			hl->addWidget(new QDoubleSpinBox(w));
		}
		{
			QLabel* l;
			vl->addWidget(new QLabel("Scale"));
			QLayout* hl = new QHBoxLayout(w);
			vl->addItem(hl);
			l = new QLabel("  X  ", w);
			l->setMaximumSize(l->sizeHint());
			hl->addWidget(l);
			hl->addWidget(new QDoubleSpinBox(w));
			l = new QLabel("  Y  ", w);
			l->setMaximumSize(l->sizeHint());
			hl->addWidget(l);
			hl->addWidget(new QDoubleSpinBox(w));
			l = new QLabel("  Z  ", w);
			l->setMaximumSize(l->sizeHint());
			hl->addWidget(l);
			hl->addWidget(new QDoubleSpinBox(w));
		}
		vl->addItem(m_window->createSpacer(Qt::Vertical));
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
	case EVENT_ADD_COMMAND_TO_COMMAND_HISTORY_GUI: //Add command to the command history list in the GUI
		{
			Event_AddCommandToCommandHistoryGUI* commandEvent = static_cast<Event_AddCommandToCommandHistoryGUI*>(e);
			Command* command = commandEvent->command;
			bool hidden = commandEvent->hidden;
			int mergeNumber = commandEvent->mergeNumber;
			
			QIcon commandIcon;
			std::string commandText = "UNKNOWN COMMAND";
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
					commandText = "Skybox toggle";
					
					std::string iconPath = ICON_PATH;
					iconPath += "Options/Skybox";
					commandIcon.addFile(iconPath.c_str());

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

			if(mergeNumber > 1)
			{
				commandText += " (" + Converter::IntToStr(mergeNumber) +")";
			}
			QString commandtextAsQString = commandText.c_str();
			QListWidgetItem* item = new QListWidgetItem(commandIcon, commandtextAsQString);
			//m_commandHistoryListWidget->insertItem(0, item); //Inserts item first (at index 0) in the list widget, automatically pushing every other item one step down
			m_commandHistoryListWidget->addItem(item);
			item->setHidden(hidden);
		}
		break;
	case EVENT_SET_SELECTED_COMMAND_GUI:
		{
			Event_SetSelectedCommandGUI* selectionEvent = static_cast<Event_SetSelectedCommandGUI*>(e);
			int index = selectionEvent->indexOfCommand;
			if(index > -1)
			{
				m_commandHistoryListWidget->setCurrentRow(index);
			}
			else
			{
				// Note: do not call "setCurrentRow" with a negative value. It will deselect all list items as intended, but then it resets itself to zero causing an unwanted SIGNAL that was harder to disconnect than when disconnected under "EVENT_REMOVE_SPECIFIED_COMMANDS_FROM_COMMAND_HISTORY_GUI". Just avoid it.

				int nrOfItems = m_commandHistoryListWidget->count();
				int itemIndex = 0;
				QListWidgetItem* item;
				do
				{
					item = m_commandHistoryListWidget->item(itemIndex);
					itemIndex++;
				}
				while(item->isHidden() && itemIndex < nrOfItems);

				// Deselect first visible command in the command history list
				item->setSelected(false);
			}
		}
		break;
	case EVENT_REMOVE_SPECIFIED_COMMANDS_FROM_COMMAND_HISTORY_GUI:
		{
			Event_RemoveCommandsFromCommandHistoryGUI* removeCommandFromGUIEvent = static_cast<Event_RemoveCommandsFromCommandHistoryGUI*>(e);
			int startAt = removeCommandFromGUIEvent->startIndex;
			int nrOfCommandsToRemove = removeCommandFromGUIEvent->nrOfCommands;

			int nrOfListItems = m_commandHistoryListWidget->count();
			if(nrOfCommandsToRemove > nrOfListItems)
			{
				nrOfCommandsToRemove = nrOfListItems;
			}

			connectCommandHistoryWidget(false);
			for(int i=startAt;i<startAt+nrOfCommandsToRemove;i++)
			{
				delete m_commandHistoryListWidget->takeItem(startAt); // "takeItem" affects current selected item of the widget, creating an unwanted SIGNAL. Therefore "connectCommandHistoryWidget(false);" is used above, to prevent the SIGNAL from being handled.
			}
			connectCommandHistoryWidget(true);
		}
		break;
	case EVENT_GET_NEXT_VISIBLE_COMMAND_ROW:
		{
			Event_GetNextOrPreviousVisibleCommandRowInCommandHistoryGUI* getEvent = static_cast<Event_GetNextOrPreviousVisibleCommandRowInCommandHistoryGUI*>(e);
			bool next = getEvent->next;

			int nrOfRows = m_commandHistoryListWidget->count();
			int currentRow = m_commandHistoryListWidget->currentRow();
			
			int addValue;
			if(next)
			{
				currentRow++;
				while(currentRow < nrOfRows-1 && currentRow > -1 && m_commandHistoryListWidget->item(currentRow)->isHidden())
				{
					currentRow++;
				}
			}
			else
			{
				currentRow--;
				while(currentRow < nrOfRows-1 && currentRow > -1 && m_commandHistoryListWidget->item(currentRow)->isHidden())
				{
					currentRow--;
				}
			}

			getEvent->row = currentRow; // Return value
		}
		break;
	case EVENT_ADD_ROOT_COMMAND_TO_COMMAND_HISTORY_GUI:
		{
			QListWidgetItem* rootCommandListItem = new QListWidgetItem("Start");
			//QColor c = QColor(1,0,0);
			//rootCommandListItem->setBackgroundColor(c);
			//rootCommandListItem->setTextAlignment(5);
			m_commandHistoryListWidget->addItem(rootCommandListItem);
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

void Manager_Docks::setupHierarchy()
{
	/*for(int i=0; i<5; i++)
	{
	QStandardItem* item;
	item = new QStandardItem("Foo");
	item->setChild(0, new QStandardItem("Fii"));
	QStandardItem* item2 = new QStandardItem("Fum");
	item2->setChild(0, new QStandardItem("Fuu"));
	item->setChild(1, item2);
	m_hierarchy->appendRow(item);
	}*/
}

void Manager_Docks::connectCommandHistoryWidget(bool connect_if_true_otherwise_disconnect)
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

void Manager_Docks::update()
{
	// Add entities to list
	DataMapper<Data::Created> map_created;
	while(map_created.hasNext())
	{
		Entity* e = map_created.nextEntity();
		e->removeData<Data::Created>();
		int entityId = e->id();

		// Make room for item
		QStandardItem* item = m_hierarchy_model->item(entityId);
		if(!item)
		{
			item = new QStandardItem();
			m_hierarchy_model->setItem(entityId, item);
		}

		// Assign item
		item->setText(e->name().c_str());
		item->setEnabled(true);
		item->setSelectable(true);

		// HACK: Make camera undeletable
		if(e->fetchData<Data::Camera>())
		{
			item->setSelectable(false);
		}
	}
	
	// Remove entries from list
	DataMapper<Data::Deleted> map_removed;
	while(map_removed.hasNext())
	{
		Entity* e = map_removed.nextEntity();
		e->removeData<Data::Deleted>();
		int entityId = e->id();

		// Assign item
		QStandardItem* item = m_hierarchy_model->item(entityId);
		if(item)
		{
			item->setEnabled(false);
			item->setSelectable(false);
		}
	}
}

void Manager_Docks::currentCommandHistoryIndexChanged(int currentRow)
{
	Event_GetCommanderInfo* commanderInfo = new Event_GetCommanderInfo(); // Retrieve information from commander
	SEND_EVENT(commanderInfo); //The event is assumed to have correct values below

	// Jump in command history if the selected command index is not already current (this check is not really needed since it is checked in Commander::tryToJumpInCommandHistory)
	int trackToCommandIndex = Converter::convertFromCommandHistoryGUIListIndexToCommandHistoryIndex(currentRow);
	if(commanderInfo->indexOfCurrentCommand != trackToCommandIndex)
	{
		SEND_EVENT(&Event_TrackToCommandHistoryIndex(trackToCommandIndex));
	}
	delete commanderInfo;
}

void Manager_Docks::selectEntity( const QModelIndex& index )
{
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
	// Fetch camera Entity

	// Allow camera to focus on the entity double-clicked on
	Entity* clickedEntity = Entity::findEntity(index.row());
}


void System_Editor::update()
{
	m_editor->update();
}

ItemBrowser::ItemBrowser( QWidget* p_parent ) : QWidget(p_parent)
{
	SUBSCRIBE_TO_EVENT(this, EVENT_REFRESH_SPLITTER);
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
		QListWidgetItem* item = new QListWidgetItem(icon, filename);
		m_grid->addItem(item);
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
	default:
		break;
	}
}

void ItemBrowser::selectEntity( QListWidgetItem* item )
{
	SETTINGS()->setSelectedTool(Enum::Tool_Geometry);
}
