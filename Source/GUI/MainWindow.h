#pragma once

#include <QTimer.h>
#include <QMainWindow>
#include <QDockWidget.h>
#include <Core/IObserver.h>

#include "ui_MainWindow.h"
#include "UpdateTimer.h"
#include "RenderWidget.h"

class Commander;
class World;

class MainWindow : public QMainWindow, public IObserver
{
	Q_OBJECT

private:
	Ui::MainWindow ui;
	QTimer refreshTimer;
	UpdateTimer updateTimer;
	RenderWidget* renderWidget;
	World* world;
	QDockWidget* sceneDock;
	std::string lastValidProjectPath;

	//QTimer soundTimer;
	//int nrOfSoundsPlayedSinceLastReset;
	Commander* commander;
	QAction* undo;
	QAction* redo;

public:
	MainWindow();
	~MainWindow();

	void onEvent(IEvent* e);
	
	void setupGame();
	void setupToolBar();
	void setupConsole();
	void setupDockWidgets();
	void computeFPS();

public slots:
	void update();
	void setFullscreen(bool checked);
	void setMaximizeScene(bool checked);
	void createDockWidget();
	void act_about();
	void setBackBufferColorToRed();
	void setBackBufferColorToGreen();
	void setBackBufferColorToBlue();
	void undoLatestCommand();
	void redoLatestCommand();
	void loadCommandHistory();
	void saveCommandHistory();
	void saveCommandHistoryAs();
	//void timer();

};