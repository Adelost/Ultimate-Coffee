#pragma once

#include <QObject.h>

class Window;
namespace Ui{class MainWindow;}
class QAction;
class QActionGroup;
class QIcon;

class Manager_Tools : QObject
{
	Q_OBJECT

private:
	Window* m_window;
	Ui::MainWindow* m_ui;
	QActionGroup* toolGroup;

public:
	void init();
	void setupToolbar();
	void setupActions();
	QAction* createContextIcon( std::string p_icon );
	QAction* createToolAction( std::string p_icon );
	QIcon createIcon( std::string p_icon );


public slots:
	void action_about();
};