#pragma once

#include <Core/ISystem.h>
#include "QObject.h"
#include <Core/IObserver.h>
#include <QDockWidget.h>
#include <QListWidget.h>

class Window;
class QDockWidget;
class QMenu;
class QAction;
class QStandardItemModel;
class QListWidget;
class Manager_Docks;
class ItemBrowser;

class Manager_Docks : public QObject, public IObserver
{
	Q_OBJECT

private:
	Window* m_window;
	QDockWidget* m_scene;
	QTreeView* m_hierarchy_tree;
	QStandardItemModel* m_hierarchy_model;
	QMenu* m_menu;
	QListWidget* m_commandHistoryListWidget;
	ItemBrowser* m_itemBrowser;

public:
	~Manager_Docks();

	void init();
	void update();
	void setupMenu();
	void setupHierarchy();

	void connectCommandHistoryWidget(bool connect_if_true_otherwise_disconnect);

	QAction* createAction(QString p_name);
	QDockWidget* createDock(QString p_name, Qt::DockWidgetArea p_area);
	ISystem* getAsSystem();
	void onEvent(Event* e);

public slots:
	void setMaximizeScene( bool p_checked );

	void createDockWidget();
	void saveLayout();
	void loadLayout();
	void resetLayout();
	void currentCommandHistoryIndexChanged(int currentRowChanged);
	void selectEntity(const QModelIndex& index);
	void focusOnEntity(const QModelIndex& index);
};


class System_Editor : public System::Type<System_Editor>
{
private:
	Manager_Docks* m_editor;
public:
	System_Editor(Manager_Docks* p_editor)
	{
		m_editor = p_editor;
	}
	void update();
};

class Item_Prefab : public QListWidgetItem
{
public:
	Item_Prefab(QIcon icon, QString filname) : QListWidgetItem(icon, filname)
	{
		static int i;
		modelId = i;
		i++;
	}
	int modelId;
};

class ItemBrowser : public QWidget, public IObserver
{
	Q_OBJECT

private:
	QListWidget* m_tree;
	QListWidget* m_grid;
	QSplitter* m_splitter;

public:
	ItemBrowser(QWidget* parent);
	void initTree();
	void onEvent(Event* e);

public slots:
	void loadGrid(QListWidgetItem* item);
	void loadGrid(int row);
	void moveHandle()
	{
		// Move splitter handle
		QList<int> sizes;
		int p = 200;
		sizes.append(p);
		sizes.append(width() - p);
		m_splitter->setSizes(sizes);
		m_splitter->refresh();
	}
	void selectEntity(QListWidgetItem* item );
};

class Hierarchy : public QTreeView
{
	Q_OBJECT

public:
	Hierarchy(QWidget* parent) : QTreeView(parent)
	{
	}

protected:
	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);
};

class ToolPanel : public QWidget
{
	Q_OBJECT

private:
	Window* m_window;
	QWidget* m_colorDialog;

public:
	ToolPanel(QWidget* parent);

public slots:
	void pickColor();
};