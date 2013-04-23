#pragma once

#include <QMainWindow>
#include <QTimer.h>
#include <QMessageBox.h>
#include <QDockWidget.h>

#include "ui_MainWindow.h"
#include "UpdateTimer.h"
#include "RenderWidget.h"

class World;

class MainWindow : public QMainWindow
{
	Q_OBJECT

private:
	Ui::MainWindowUi ui;
	QTimer refreshTimer;
	UpdateTimer updateTimer;
	RenderWidget* renderWidget;
	World* world;

public:
	MainWindow();
	~MainWindow();
	
	void setupGame();
	void setupToolBar();
	void setupConsole();
	void setupDockWidgets();
	void computeFPS();

public  slots:
	void update();
	void act_about();


};