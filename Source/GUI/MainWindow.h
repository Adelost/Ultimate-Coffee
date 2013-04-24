#pragma once

#include <QMainWindow>
#include <QTimer.h>
#include <QMessageBox.h>
#include <QDockWidget.h>

#include "ui_MainWindow.h"
#include "UpdateTimer.h"
#include "RenderWidget.h"

class Commander;
class World;

class MainWindow : public QMainWindow
{
	Q_OBJECT

private:
	Ui::MainWindow ui;
	QTimer refreshTimer;
	UpdateTimer updateTimer;
	RenderWidget* renderWidget;
	World* world;

	std::string lastValidProjectPath;
	Commander* commander;

public:
	MainWindow();
	~MainWindow();
	
	void setupGame();
	void setupToolBar();
	void setupConsole();
	void setupDockWidgets();
	void computeFPS();

public slots:
	void update();
	void act_about();
	void setBackBufferColorToRed();
	void setBackBufferColorToGreen();
	void setBackBufferColorToBlue();
	void undoLatestCommand();
	void redoLatestCommand();
	void loadCommandHistory();
	void saveCommandHistory();
	void saveCommandHistoryAs();

};