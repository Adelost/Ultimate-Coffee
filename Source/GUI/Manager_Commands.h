#pragma once

#include <Core/IObserver.h>
#include "QObject.h"

class Window;
namespace Ui{class MainWindow;}
class QToolBar;
class Commander;
class CommanderSpy;
class QAction;
class QSignalMapper;

class Manager_Commands
	: QObject, public IObserver
{
	Q_OBJECT

private:
	Window* m_window;
	Ui::MainWindow* m_ui;
	QToolBar* m_toolbar_commands;
	Commander* m_commander;
	CommanderSpy* m_commanderSpy;
	std::string m_lastValidProjectPath;
	//QTimer soundTimer;
	//int nrOfSoundsPlayedSinceLastReset;

	void storeCommandInCommandHistory(Command* command, bool execute, int nrOfJumps, bool hiddenInGUIList, int GUI_MergeNumber = 0);
	void jumpInCommandHistory(int commandHistoryIndex);
	void updateCurrentCommandGUI();

public:
	void init();
	~Manager_Commands();
	void setupMenu();

public slots:
	void redoLatestCommand();
	void undoLatestCommand();

	void saveCommandHistory();
	void saveCommandHistoryAs();
	void loadCommandHistory();
	
	void createTestButton( QString color, QSignalMapper* mapper );
	void setBackBufferColor(QString p_str_color);

	void translateSceneEntity();

	void onEvent(IEvent* e);
};