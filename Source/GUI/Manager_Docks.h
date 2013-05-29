#pragma once

#include <Core/ISystem.h>
#include "QObject.h"
#include <Core/IObserver.h>
#include <QDockWidget.h>
#include <QListWidget.h>
#include <Core\Serializable.h>

//#include <math.h>

class Window;
class QDockWidget;
class QMenu;
class QAction;
class QStandardItemModel;
class QListWidget;
class Manager_Docks;
class ItemBrowser;
class QColorDialog;

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

	void connectCommandHistoryListWidget(bool connect_if_true_otherwise_disconnect);

	// Adds a "ListItemWithIndex" to a "QListWidget"
	void addItemToCommandHistoryListWidget(const QIcon& icon, const QString& text, int index, int nrOfCommandsRepresented);
	
	// Example: the command which in the "CommandHistory" is at index 5, what index does it have in the "QListWidget"
	int findListItemIndexFromCommandHistoryIndex(int commandHistoryIndex);

	// Only send "QListWidgetItem" that really are "ListItemWithIndex" as argument
	int getIndexFromItemWithIndex(QListWidgetItem* item);

	void playDingSound();

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
	void currentCommandHistoryIndexChanged(int currentRow);
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
	Item_Prefab(QIcon icon, QString filname);
	Enum::Mesh mesh;
	Color color;
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

class ToolPanel : public QWidget, IObserver
{
	Q_OBJECT

private:
	Window* m_window;
	QColorDialog* m_colorDialog;
	QLabel* m_colorIcon;

	QDoubleSpinBox	*translationXSpinBox,	*translationYSpinBox,	*translationZSpinBox,
					*rotationXSpinBox,		*rotationYSpinBox,		*rotationZSpinBox,
					*scalingXSpinBox,		*scalingYSpinBox,		*scalingZSpinBox;

	bool spinboxValueSetBecauseOfSelectionOrTransformation; //SoDoNotSetValueForAllObjects;

public:
	ToolPanel(QWidget* parent);
	void onEvent(Event* p_event);

public slots:
	void pickColor();
	void setColor(const QColor& color);

	void setXTranslationOfSelectedEntities(double p_transX);
	void setYTranslationOfSelectedEntities(double p_transY);
	void setZTranslationOfSelectedEntities(double p_transZ);

	void setXScalingOfSelectedEntities(double p_scaleX);
	void setYScalingOfSelectedEntities(double p_scaleY);
	void setZScalingOfSelectedEntities(double p_scaleZ);

	void setXRotationOfSelectedEntities(double p_rotationX);
	void setYRotationOfSelectedEntities(double p_rotationY);
	void setZRotationOfSelectedEntities(double p_rotationZ);
};

class ListWidgetWithoutKeyboardInput : public QListWidget
{
	Q_OBJECT

public:
	ListWidgetWithoutKeyboardInput(QWidget* parent) : QListWidget(parent){}

protected:
	void keyPressEvent(QKeyEvent *e)
	{

	}
	void keyReleaseEvent(QKeyEvent *e)
	{

	}
};

class ListItemWithIndex : public QListWidgetItem, public Serializable
{
private:
	struct DataStruct
	{
		int commandHistoryIndex; // Defines the mapping between a command in "CommandHistory" and a QListWidgetItem (ListItemWithIndex) in a QListWidget.
		int nrOfCommandsRepresented; // Needed when saving and loading GUIFilter, refer to //check. Example when nrOfCommandsRepresented=27 "Entity creation (27)".
	};
	DataStruct m_dataStruct;

public:
	ListItemWithIndex(const QIcon& icon, const QString& text, int index, int nrOfCommandsRepresented);
	int getCommandHistoryIndex();

	// pure virtual overrides from "Serializable"
	void* accessDerivedClassDataStruct(){return reinterpret_cast<void*>(&m_dataStruct);}
	virtual int getByteSizeOfDataStruct(){return sizeof(m_dataStruct);}
	virtual void loadDataStructFromBytes(char* data){m_dataStruct = *reinterpret_cast<DataStruct*>(&m_dataStruct);}
};