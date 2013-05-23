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
	QAction* m_action_skybox;
	//QTimer soundTimer;
	//int nrOfSoundsPlayedSinceLastReset;

	bool storeCommandInCommandHistory(Command* command, bool execute);
	void updateCommandHistoryGUI(Command* command, bool hiddenInGUIList, int GUI_MergeNumber, int nrOfCommandsBeforeAdd);
	bool jumpInCommandHistory(int commandHistoryIndex);
	void updateCurrentCommandGUI();
	void loadCommandHistory(std::string path);
	void saveCommandHistory(std::string path);

public:
	void init();
	~Manager_Commands();
	void setupMenu();

public slots:
	void redoLatestCommand();
	void undoLatestCommand();

	void quicksaveCommandHistory();
	void saveProjectFileDialog();
	void loadProjectFileDialog();
	void loadRecentCommandHistory();
	
	void createTestButton( QString color, QSignalMapper* mapper );
	void setBackBufferColor(QString p_str_color);

	void translateSceneEntity();
	void enableSkybox(bool state);

	void onEvent(Event* e);
};