#pragma once

#include "Util.h"
class Window;
namespace Ui{class MainWindow;}


class Manager_Tools : QObject
{
	Q_OBJECT

private:
	Window* m_window;
	Ui::MainWindow* m_ui;

public:
	void init();
	void setupToolbar();
	void setupActions();
	void createContextIcon( std::string p_icon );
	QAction* createToolIcon( std::string p_icon );


public slots:
	void action_about();
};