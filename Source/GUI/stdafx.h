#pragma once

#include <Core/stdafx.h>//check, interdependent precompiled headers
#include <Core/IObserver.h>
#include <Core/Converter.h>

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
#include <QListwidget.h>
#include <QBoxLayout.h>
#include <QDoubleSpinBox>

// Path to icons
#define ICON_PATH												\
	"../../Assets/GUI/Icons/"