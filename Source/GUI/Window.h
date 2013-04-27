#pragma once

#include "Util.h"
#include "Manager_Tools.h"
#include "Manager_Console.h"
#include "Manager_Commands.h"
#include "Manager_Docks.h"
#include "Manager_Tools.h"
#include "ui_MainWindow.h"
class UpdateLoop;


class Window : public QMainWindow, public IObserver
{
	Q_OBJECT

private:
	Ui::MainWindow m_ui;
	Manager_Commands m_manager_commands;
	Manager_Console m_manager_console;
	Manager_Docks m_manager_docks;
	Manager_Tools m_manager_tools;
	QTimer m_refreshTimer;
	UpdateLoop* m_updateLoop;

private:
	Window();

public:
	static Window* instance();
	~Window();

	void onEvent(IEvent* p_event);
	Ui::MainWindow* ui();
	QIcon createIcon(QColor p_color);

public slots:
	void update();
	void setFullscreen(bool p_checked);

};