#pragma once

#include <QObject.h>

class Window;
namespace Ui{class MainWindow;}
class QActionGroup;
class QAction;
class QIcon;
class QSignalMapper;
class IEvent;

enum ToolType
{
	NONE,
	SELECTION,
	TRANSLATE,
	ROTATE,
	SCALE,
	GEOMETRY,
	ENTITY
};


class Manager_Tools : public QObject, public IObserver
{
	Q_OBJECT

private:
	Window* m_window;
	Ui::MainWindow* m_ui;
	QActionGroup* m_toolGroup;
	ToolType m_selectedTool;

public:
	void init();
	void onEvent(IEvent* p_event);

	void setupToolbar();
	void setupActions();
	QAction* createContextIcon(std::string p_icon);
	QAction* createToolAction(QSignalMapper* p_mapper, ToolType p_type, std::string p_icon);
	QIcon createIcon( std::string p_icon );

public slots:
	void action_about();
	void setTool(int p_toolType);
};