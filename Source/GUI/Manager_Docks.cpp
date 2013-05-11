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

Manager_Docks::~Manager_Docks()
{
}

void Manager_Docks::init()
{
	SUBSCRIBE_TO_EVENT(this, EVENT_ADD_COMMAND_TO_COMMAND_HISTORY_GUI);
	SUBSCRIBE_TO_EVENT(this, EVENT_SET_SELECTED_COMMAND_GUI);
	SUBSCRIBE_TO_EVENT(this, EVENT_REMOVE_SPECIFIED_COMMANDS_FROM_COMMAND_HISTORY_GUI);
	commandHistoryListWidget = nullptr;
	listT = nullptr;
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

	// Command history
	dock = createDock("Command history (Should be fully functional as of 2013-05-07 04.07).", Qt::LeftDockWidgetArea);
	commandHistoryListWidget = new QListWidget(dock);
	connectCommandHistoryWidget(true);
	dock->setWidget(commandHistoryListWidget);

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
		QWidget* widget = new QWidget(dock);
		dock->setWidget(widget);
		QLayout* vl = new QVBoxLayout(widget);
		widget->setLayout(vl);
		{
			vl->addWidget(new QLabel("Position"));
			QLayout* hl = new QHBoxLayout(widget);
			vl->addItem(hl);
			hl->addWidget(new QLabel("  X ", widget));
			hl->addWidget(new QDoubleSpinBox(widget));
			hl->addWidget(new QLabel("  Y ", widget));
			hl->addWidget(new QDoubleSpinBox(widget));
			hl->addWidget(new QLabel("  Z ", widget));
			hl->addWidget(new QDoubleSpinBox(widget));
		}
		widget->setLayout(vl);
		{
			vl->addWidget(new QLabel("Rotation"));
			QLayout* hl = new QHBoxLayout(widget);
			vl->addItem(hl);
			hl->addWidget(new QLabel("  X ", widget));
			hl->addWidget(new QDoubleSpinBox(widget));
			hl->addWidget(new QLabel("  Y ", widget));
			hl->addWidget(new QDoubleSpinBox(widget));
			hl->addWidget(new QLabel("  Z ", widget));
			hl->addWidget(new QDoubleSpinBox(widget));
		}
		widget->setLayout(vl);
		{
			vl->addWidget(new QLabel("Scale"));
			QLayout* hl = new QHBoxLayout(widget);
			vl->addItem(hl);
			hl->addWidget(new QLabel("  X ", widget));
			hl->addWidget(new QDoubleSpinBox(widget));
			hl->addWidget(new QLabel("  Y ", widget));
			hl->addWidget(new QDoubleSpinBox(widget));
			hl->addWidget(new QLabel("  Z ", widget));
			hl->addWidget(new QDoubleSpinBox(widget));
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

void Manager_Docks::onEvent(IEvent* e)
{
	EventType type = e->type();
	switch (type) 
	{
	case EVENT_ADD_COMMAND_TO_COMMAND_HISTORY_GUI: //Add command to the command history list in the GUI
		{
			Event_AddCommandToCommandHistoryGUI* commandEvent = static_cast<Event_AddCommandToCommandHistoryGUI*>(e);
			Command* command = commandEvent->command;
			
			QIcon commandIcon;
			QString commandText = "UNKNOWN COMMAND";
			Enum::CommandType type = command->getType();
			switch(type)
			{
			case Enum::CommandType::CHANGEBACKBUFFERCOLOR:
				{
					commandText = "Backbuffer color";
					Command_ChangeBackBufferColor* changeBackBufferColorEvent = static_cast<Command_ChangeBackBufferColor*>(command);
				
					float x = changeBackBufferColorEvent->getDoColorX();
					float y = changeBackBufferColorEvent->getDoColorY();
					float z = changeBackBufferColorEvent->getDoColorZ();

					QColor color(x,y,z);
					QPixmap pixmap(16, 16);
					pixmap.fill(color);
					commandIcon.addPixmap(pixmap);
					break;
				}
			case Enum::CommandType::TRANSLATE_SCENE_ENTITY:
				{
					commandText = "Translate";
					Command_TranslateSceneEntity* translateSceneEntityEvent = static_cast<Command_TranslateSceneEntity*>(command);

					// Could have the translation tool icon be displayed, instead of a color, perhaps.

					float r = 65.0f;
					float g = 65.0f;
					float b = 65.0f;

					QColor color(r, g, b);
					QPixmap pixmap(16, 16);
					pixmap.fill(color);
					commandIcon.addPixmap(pixmap);
					break;
				}
			default:
				{
					std::string iconPath = ICON_PATH;
					iconPath += "Menu/save";
					commandIcon.addFile(iconPath.c_str());
					break;
				}
			}

			QListWidgetItem* item = new QListWidgetItem(commandIcon, commandText);
			commandHistoryListWidget->insertItem(0, item); //Inserts item first (at index 0) in the list widget, automatically pushing every other item one step down
			//commandHistoryListWidget->setItemSelected(item, true);

			//int nrOfListItems = commandHistoryListWidget->count();

			// QListWidget
			// to remove items from commandHistoryListWidget,
			// use commandHistoryListWidget->takeItem(index);

			//new QListWidgetItem(QIcon(iconPath.c_str()), commandList.at(i).c_str(), commandHistoryListWidget);
			//commandHistoryListWidget->sortItems(Qt::SortOrder::DescendingOrder); //check performance hit if long list
			//commandHistoryListWidget->setViewMode(QListView::ViewMode::IconMode);
			//commandHistoryListWidget->setLayoutMode(QListView::LayoutMode::SinglePass);
			//commandHistoryListWidget->setIconSize(QSize(10000, 10000));
		break;
		}
	case EVENT_SET_SELECTED_COMMAND_GUI:
		{
			Event_SetSelectedCommandGUI* selectionEvent = static_cast<Event_SetSelectedCommandGUI*>(e);
			int index = selectionEvent->indexOfCommand;

			if(index == -1) // Special case: jump out of history
			{
				commandHistoryListWidget->item(commandHistoryListWidget->count()-1)->setSelected(false);
			}
			else
			{
				commandHistoryListWidget->setCurrentRow(index);
			}

			break;
		}
	case EVENT_REMOVE_SPECIFIED_COMMANDS_FROM_COMMAND_HISTORY_GUI:
		{
			Event_RemoveCommandsFromCommandHistoryGUI* removeCommandFromGUIEvent = static_cast<Event_RemoveCommandsFromCommandHistoryGUI*>(e);
			int startAt = removeCommandFromGUIEvent->startIndex;
			int nrOfCommandsToRemove = removeCommandFromGUIEvent->nrOfCommands;

			connectCommandHistoryWidget(false);
			for(int i=startAt;i<startAt+nrOfCommandsToRemove;i++)
			{
				delete commandHistoryListWidget->takeItem(startAt); //takeItem affects current selected item of the widget, creating an unwanted SIGNAL. Therefore "connectCommandHistoryWidget(false);" is used above, to prevent the SIGNAL from being handled.
			}
			connectCommandHistoryWidget(true);

			break;
		}
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
		connect(commandHistoryListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(currentCommandHistoryIndexChanged(int)));
	}
	else
	{
		disconnect(commandHistoryListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(currentCommandHistoryIndexChanged(int)));
	}
}

void Manager_Docks::update()
{
	int rowCount = m_hierarchy_model->rowCount();
	int entityCount = 0;

	DataMapper<Data::Transform> map_trans;
	while(map_trans.hasNext())
	{
		map_trans.next();

		if(entityCount >= rowCount && entityCount < 200)
		{
			QStandardItem* item;
			item = new QStandardItem("Entity " + QString::number(entityCount));
			m_hierarchy_model->setItem(entityCount, item);
		}
		entityCount++;
	}
}

void Manager_Docks::currentCommandHistoryIndexChanged(int currentRow)
{
	Event_GetCommanderInfo* commanderInfo = new Event_GetCommanderInfo(); // Retrieve information from commander
	SEND_EVENT(commanderInfo); //The event is assumed to have correct values below

	//Jump in command history if the selected command index is not already current (this check is not really needed since it is checked in Commander::tryToJumpInCommandHistory)
	int trackToCommandIndex = Converter::convertBetweenCommandHistoryIndexAndGUIListIndex(currentRow, commanderInfo->nrOfCommands);
	if(commanderInfo->indexOfCurrentCommand != trackToCommandIndex)
	{
		SEND_EVENT(&Event_TrackToCommandHistoryIndex(trackToCommandIndex));
	}
	delete commanderInfo;
}

void Manager_Docks::selectEntity( const QModelIndex & index )
{
	DataMapper<Data::Selected> map_selected;

	// Remove previous selection
	Data::Selected::clearSelection();

	// Add new selection
	QList<QModelIndex> index_list = m_hierarchy_tree->selectionModel()->selectedRows();
	foreach(QModelIndex index, index_list)
	{
		int entityId = index.row();
		Entity* e = Entity::findEntity(entityId);
		e->addData(Data::Selected());
	}

	// Debug selection
	DEBUGPRINT("SELECTED: " + Converter::IntToStr(map_selected.dataCount()));
	while(map_selected.hasNext())
	{
		Entity* e = map_selected.nextEntity();
		DEBUGPRINT(" Entity: " + Converter::IntToStr(e->id()));
	}
}


void System_Editor::update()
{
	m_editor->update();
}

//DataMapper<Data::Selected> map_selected;
//
//// Remove previous selection
//Data::Selected::clearSelection();
//m_hierarchy_view->setCurrentIndex(index);
//
//// Add new selection
//QList<QModelIndex> index_list = m_hierarchy_view->selectionModel()->selectedRows();
//
//// Select clicked item
//Entity* picked_entity = Entity::findEntity(index.row());
//DEBUGPRINT(" Found: " + Converter::IntToStr(picked_entity->id()));
//picked_entity->addData(Data::Selected());
//
//// Select from selection
//foreach(QModelIndex index, index_list)
//{
//	int entityId = index.row();
//	Entity* e = Entity::findEntity(entityId);
//	DEBUGPRINT(" Found: " + Converter::IntToStr(e->id()));
//	e->addData(Data::Selected());
//}
//
//// Debug selection
//DEBUGPRINT("\nSELECTION: " + Converter::IntToStr(map_selected.dataCount()));
//while(map_selected.hasNext())
//{
//	Entity* e = map_selected.nextEntity();
//	DEBUGPRINT(" Entity: " + Converter::IntToStr(e->id()));
//}