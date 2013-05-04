#pragma once

#include <QWidget>
#include <Core/IObserver.h>
#include <QKeyEvent>
#include <QMouseEvent> // needed to grabb mouse input
class IEvent;

class RenderWidget : public QWidget, public IObserver
{
	Q_OBJECT

public:
	RenderWidget(QWidget* parent);
	~RenderWidget();

	void onEvent(IEvent* p_event);

	 // Overrides Qt:s own paint engine. Prevents flicker.
	QPaintEngine* paintEngine() const {return 0;}

protected:
	// should not be implemented
	void paintEvent(QPaintEvent* e){}


protected:
	void mousePressEvent(QMouseEvent* p_event);
	void mouseReleaseEvent(QMouseEvent* p_event);
	void resizeEvent(QResizeEvent* e);
	void mouseMoveEvent(QMouseEvent *e);
	void keyPressEvent(QKeyEvent *e);
};