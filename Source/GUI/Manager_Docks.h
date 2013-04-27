#pragma once

#include "Util.h"
#include "RenderWidget.h"
class Window;


class Manager_Docks : QObject
{
	Q_OBJECT

private:
	Window* m_window;
	QDockWidget* m_scene;
	QMenu* m_menu;
	RenderWidget* m_renderWidget;

public:
	~Manager_Docks();

	void init();
	void saveLayout();
	void loadLayout();
	void setupMenu();

public slots:
	void setMaximizeScene( bool p_checked );

	void createDockWidget();
};