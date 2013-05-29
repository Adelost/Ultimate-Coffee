#pragma once

#include <Core/IObserver.h>
#include "QObject.h"

class Window;
namespace Ui{class MainWindow;}
class QToolBar;
class CommandHistory;
class CommandHistorySpy;
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
	CommandHistory* m_commandHistory;
	CommandHistorySpy* m_commandHistorySpy;
	std::string m_lastValidProjectPath;
	QAction* m_action_skybox;
	QAction* m_action_skybox2;
	QActionGroup* m_skyboxGroup;

	bool storeCommandsInCommandHistory(std::vector<Command*>* commands, bool execute);
	void updateCommandHistoryGUI(std::vector<Command*>* commands, bool displayAsSingleCommandHistoryEntry);
	bool jumpInCommandHistory(int commandHistoryIndex);
	void updateCurrentCommandGUI();
	void newProject();
	void loadCommandHistory(std::string path);
	void saveCommandHistory(std::string path);
	void saveCommandHistoryGUIFilter(std::string path);
	void loadCommandHistoryGUIFilter(std::string path);
	void clearCommandHistoryGUI();
	int tryToGetFileSize(std::string path);

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
	
	QAction* createTestButton( QString color, QSignalMapper* mapper );
	void setBackBufferColor(QString p_str_color);

	void translateSceneEntity();
	void enableSkybox(bool state);

	void onEvent(Event* e);
};