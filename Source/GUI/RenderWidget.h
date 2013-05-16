#pragma once

#include <QWidget>
#include <Core/IObserver.h>
#include <QKeyEvent>
#include <QMouseEvent> // needed to grabb mouse input
class Event;

class RenderWidget : public QWidget, public IObserver
{
	Q_OBJECT

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