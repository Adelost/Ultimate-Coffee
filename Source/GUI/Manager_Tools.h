#pragma once

#include <QObject.h>
//#include <Core/Enums.h>

class Window;
namespace Ui{class MainWindow;}
class QActionGroup;
class QAction;
class QIcon;
class QSignalMapper;
class Event;

class Manager_Tools : public QObject, public IObserver
{
	Q_OBJECT

private:
	Window* m_window;
	Ui::MainWindow* m_ui;
	QActionGroup* m_toolGroup;
	int m_selectedTool;

public:
	void init();
	void onEvent(Event* p_event);

	void setupToolbar();
	void setupActions();
	QAction* createContextIcon(std::string p_icon);
	QAction* createToolAction(QSignalMapper* p_mapper, int p_type, std::string p_icon);
	QIcon createIcon( std::string p_icon );

public slots:
	void action_about();
	void setTool(int p_toolType);
	void coffee();
	void createAsteroids();
	void createAsteroid();
	void newLevel();
};