#pragma once

#include <QTimer.h>
#include <QMainWindow>
#include <QDockWidget.h>
#include <Core/IObserver.h>

#include "ui_MainWindow.h"
#include "UpdateTimer.h"
#include "RenderWidget.h"

class Commander;
class UpdateLoop;

class MainWindow : public QMainWindow, public IObserver
{
	Q_OBJECT

private:
	Ui::MainWindow m_ui;
	//QTimer soundTimer;
	//int nrOfSoundsPlayedSinceLastReset;
	Commander* m_commander;
	QAction* m_action_redo;
	QAction* m_action_undo;
	QDockWidget* m_sceneDock;
	QTimer m_refreshTimer;
	RenderWidget* m_renderWidget;
	UpdateLoop* m_updateLoop;
	UpdateTimer m_updateTimer;
	std::string m_lastValidProjectPath;



public:
	MainWindow();
	~MainWindow();

	void onEvent(IEvent* p_event);
	void computeFPS();
	void setupConsole();
	void setupDockWidgets();
	void setupGame();
	void setupToolBar();

public slots:
	void update();
	void action_about();
	void createDockWidget();
	void loadCommandHistory();
	void redoLatestCommand();
	void saveCommandHistory();
	void saveCommandHistoryAs();
	void setBackBufferColorToBlue();
	void setBackBufferColorToGreen();
	void setBackBufferColorToRed();
	void setFullscreen(bool p_checked);
	void setMaximizeScene(bool p_checked);
	void undoLatestCommand();
	//void timer();

};