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


class SplashScreen : public QMessageBox
{
	Q_OBJECT
		
public:
	SplashScreen( QWidget* parent ) : QMessageBox(parent)
	{
		// Create window in middle of screen
		setWindowTitle("Welcome");
		setText("Welcome to Ultimate Coffee(tm)");
		//setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
		show();
		setModal(false);
		
		QLabel* l = new QLabel("");
		std::string path = "";
		path = path + ICON_PATH + "Misc/" + "spash";
		l->setPixmap(QPixmap(path.c_str()));
		layout()->addWidget(l);
		resize(sizeHint());
		move(parent->x() + parent->x()/2 + width(), parent->y() + parent->y()/2 - height()/2);
	}
	~SplashScreen()
	{
		int i = 0;
		i = 0;
	}
};

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
};