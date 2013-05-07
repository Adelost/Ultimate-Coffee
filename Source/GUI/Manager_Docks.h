#pragma once

#include <Core/ISystem.h>
#include "QObject.h"
#include <Core/IObserver.h>
#include <QDockWidget.h>
class Window;
class QDockWidget;
class QMenu;
class QAction;
class QStandardItemModel;
class QListWidget;
class QListView;
class Manager_Docks;

class Manager_Docks : public QObject, public IObserver
{
	Q_OBJECT

private:
	Window* m_window;
	QDockWidget* m_scene;
	QStandardItemModel* m_hierarchy;
	QMenu* m_menu;
	QListWidget* commandHistoryListWidget;
	QListView* listT;

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
	void onEvent(IEvent* e);

public slots:
	void setMaximizeScene( bool p_checked );

	void createDockWidget();
	void saveLayout();
	void loadLayout();
	void resetLayout();
	void currentCommandHistoryIndexChanged(int currentRowChanged);
	void selectEntity( const QModelIndex & index);
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