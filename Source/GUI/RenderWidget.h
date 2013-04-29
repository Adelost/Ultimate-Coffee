#pragma once

#include <QWidget>
#include <Core/IObserver.h>
#include <Core/Events.h>
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
	void resizeEvent(QResizeEvent* e)
	{
		QWidget::resizeEvent(e);

		SEND_EVENT(&Event_WindowResize(width(), height()));
	}

protected:
	void mousePressEvent(QMouseEvent* p_event);

	void mouseReleaseEvent(QMouseEvent* p_event);
};