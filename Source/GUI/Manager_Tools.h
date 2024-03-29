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
	QAction* createToolAction(QSignalMapper* mapper, int type, std::string icon, std::string toolTip);
	QIcon createIcon( std::string p_icon );

public slots:
	void action_about();
	void action_copy();
	void action_paste();
	void setTool(int p_toolType);
	void coffee();
	void createAsteroids();
	void createAsteroid();
	void newProject();
	void runSimulation(bool state);
	void doFrustumCulling(bool state);
	void homingAsteroids(bool state);
	void loadImage();
	void previewItemBrowser();
};