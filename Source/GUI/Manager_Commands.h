#pragma once

#include <Core/Commander.h>
#include <Core/World.h>
#include "Util.h"
class Window;
namespace Ui{class MainWindow;}


class Manager_Commands : QObject
{
	Q_OBJECT

private:
	Window* m_window;
	Ui::MainWindow* m_ui;
	QToolBar* m_toolbar_commands;
	Commander* m_commander;
	QAction* m_action_redo;
	QAction* m_action_undo;
	std::string m_lastValidProjectPath;
	//QTimer soundTimer;
	//int nrOfSoundsPlayedSinceLastReset;

public:
	void init();
	~Manager_Commands();
	void setupMenu();

public slots:
	void loadCommandHistory();
	void redoLatestCommand();

	void saveCommandHistory();
	void saveCommandHistoryAs();
	void undoLatestCommand();

	void createTestButton( QString color, QSignalMapper* mapper );
	void setBackBufferColor(QString p_str_color);
};