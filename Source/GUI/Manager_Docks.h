#pragma once

#include "QObject.h"

class Window;
class QDockWidget;
class QMenu;
class QAction;
class QStandardItemModel;
class QListWidget;

class Manager_Docks : QObject
{
	Q_OBJECT

private:
	Window* m_window;
	QDockWidget* m_scene;
	QStandardItemModel* m_hierarchy;
	QMenu* m_menu;
	QListWidget* commandHistoryListWidget;
	//std::vector<QListWidgetItem*> commandHistoryList;

public:
	~Manager_Docks();

	void init();
	void setupMenu();
	void setupHierarchy();

	QAction* createAction(QString p_name);
	QDockWidget* createDock(QString p_name, Qt::DockWidgetArea p_area);

public slots:
	void setMaximizeScene( bool p_checked );

	void createDockWidget();
	void saveLayout();
	void loadLayout();
	void resetLayout();
};