#pragma once

#include <Core/Converter.h>
#include <Core/Data.h>
#include <Core/Entity.h>
#include <Core/EventManager.h>
#include <Core/Events.h>
#include <Core/Factory_Entity.h>
#include <Core/IObserver.h>
#include <Core/Settings.h>
#include <Core/World.h>
#include <Core/Util.h>

#include <QAction>
#include <QDockWidget.h>
#include <QFileDialog>
#include <QGraphicsBlurEffect>
#include <QLabel.h>
#include <QMainWindow.h>
#include <QMenu.h>
#include <QMessageBox.h>
#include <QPlainTextEdit>
#include <QShortcut.h>
#include <QSignalMapper>
#include <QSound>
#include <QStandardItemModel>
#include <QString>
#include <QTimer.h>
#include <QTreeView>
#include <QListWidget.h>
#include <QBoxLayout.h>
#include <QDoubleSpinBox>
#include <QScrollArea>
#include <QPushButton>
#include <QDir>
#include <QSplitter>


// Paths
#define ICON_PATH												\
	"../../Assets/GUI/Icons/"
#define THUMBNAIL_PATH												\
	"../../Assets/GUI/Thumbnails/"