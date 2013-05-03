#pragma once

#include <Core/ISystem.h>
#include "QObject.h"

class Window;
class QDockWidget;
class QMenu;
class QAction;
class QStandardItemModel;
class QListWidget;
class Manager_Docks;

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

class Manager_Docks : public QObject
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
	void update();
	void setupMenu();
	void setupHierarchy();

	QAction* createAction(QString p_name);
	QDockWidget* createDock(QString p_name, Qt::DockWidgetArea p_area);
	ISystem* getAsSystem()
	{
		return new System_Editor(this);
	}

public slots:
	void setMaximizeScene( bool p_checked );

	void createDockWidget();
	void saveLayout();
	void loadLayout();
	void resetLayout();
};