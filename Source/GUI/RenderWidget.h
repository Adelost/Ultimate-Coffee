#pragma once

#include <QWidget>
#include <Core/IObserver.h>
#include <QKeyEvent>
#include <QMouseEvent> // needed to grabb mouse input
class QRubberBand;
class Event;
class Tool_MultiSelect;

class RenderWidget : public QWidget, public IObserver
{
	Q_OBJECT

private:
	Tool_MultiSelect* multiSelect;

public:
	RenderWidget(QWidget* p_parent);
	~RenderWidget();
	QPoint prevMousePos;

	void onEvent(Event* p_event);

	 // Overrides Qt:s own paint engine. Prevents flicker.
	QPaintEngine* paintEngine() const {return 0;}

protected:
	// should not be implemented
	void paintEvent(QPaintEvent* p_event){}


protected:
	void mousePressEvent(QMouseEvent* p_event);
	void mouseReleaseEvent(QMouseEvent* p_event);
	void resizeEvent(QResizeEvent* p_event);
	void mouseMoveEvent(QMouseEvent* p_event);
	void keyPressEvent(QKeyEvent* p_event);
	void keyReleaseEvent(QKeyEvent* p_event);
	void setKeyState(QKeyEvent* p_event, bool p_pressed);
	void setMouseState(QMouseEvent* p_event, bool p_pressed);
};

class Tool_MultiSelect : public QObject, public IObserver
{
	Q_OBJECT

public:
	QRubberBand* rubberBand;
	Int2 origin;
	bool used;

public:
	Tool_MultiSelect(QWidget* parent);
	void onEvent(Event* e);
};