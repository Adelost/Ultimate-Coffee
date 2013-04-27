#pragma once

#include "QObject.h"

class RenderWidget;
class Window;
class QDockWidget;
class QMenu;

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