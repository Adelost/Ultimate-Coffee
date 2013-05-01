#pragma once

#include <QMainWindow.h>
#include <Core/IObserver.h>
#include <QObject.h>

namespace Ui{class MainWindow;}
class Manager_Commands;
class Manager_Console;
class Manager_Docks;
class Manager_Tools;
class QTimer;
class UpdateLoop;
class IEvent;
class QColor;
class QSpacerItem;

class Window : public QMainWindow, public IObserver
{
	Q_OBJECT

private:
	Ui::MainWindow* m_ui;
	Manager_Commands* m_manager_commands;
	Manager_Console* m_manager_console;
	Manager_Docks* m_manager_docks;
	Manager_Tools* m_manager_tools;
	QTimer* m_refreshTimer;
	UpdateLoop* m_updateLoop;
	QWidget* m_renderWidget;

private:
	Window();

public:
	static Window* instance();
	~Window();

	void onEvent(IEvent* p_event);
	Ui::MainWindow* ui();
	QIcon createIcon(QColor* p_color);
	QSpacerItem* createSpacer(Qt::Orientation p_orientation);

	QWidget* renderWidget();

public slots:
	void update();
	void setFullscreen(bool p_checked);

};