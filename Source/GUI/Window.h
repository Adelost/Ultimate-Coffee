#pragma once

#include <QMainWindow.h>
#include <QMessageBox.h>
#include <Core/IObserver.h>
#include <QObject.h>

namespace Ui{class MainWindow;}
class Manager_Commands;
class Manager_Console;
class Manager_Docks;
class Manager_Tools;
class QTimer;
class UpdateLoop;
class Event;
class QColor;
class QSpacerItem;
class ISystem;




class Window : public QMainWindow, public IObserver
{
	Q_OBJECT

private:
	static Window* s_instance;
	Ui::MainWindow* m_ui;
	Manager_Commands* m_manager_commands;
	Manager_Console* m_manager_console;
	Manager_Docks* m_manager_docks;
	Manager_Tools* m_manager_tools;
	QTimer* m_refreshTimer;
	UpdateLoop* m_updateLoop;
	QWidget* m_renderWidget;

private:
	Window(){}

public:
	static Window* instance();
	void init();
	~Window();

	void onEvent(Event* p_event);
	Ui::MainWindow* ui();
	QIcon createIcon(QColor* p_color);
	QSpacerItem* createSpacer(Qt::Orientation p_orientation);
	void setRefreshInterval(int p_interval);

	QWidget* renderWidget();
	ISystem* system_editor();
	bool eventFilter(QObject* object, QEvent* event);

public slots:
	void update();
	void setFullscreen(bool p_checked);

protected:
	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);
};

class SplashScreen : public QDockWidget
{
	Q_OBJECT

public:
	SplashScreen( Window* parent );
	~SplashScreen();

public slots:
	void newProject();
	void loadProject();
	void loadRecentProject();
	void changeColorScheme(int enum_scheme);
};