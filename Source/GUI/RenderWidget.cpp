#include "stdafx.h"
#include "RenderWidget.h"
#include <Core/Events.h>
#include <Core/World.h>

RenderWidget::RenderWidget( QWidget* parent ) : QWidget(parent)
{
	// make widget non-transparent & draw directly onto screen
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_PaintOnScreen);
	//setStyleSheet("background-color: rgba(0, 0, 0, 255);");

	SUBSCRIBE_TO_EVENT(this, EVENT_GET_WINDOW_HANDLE);
}

RenderWidget::~RenderWidget()
{
}

void RenderWidget::onEvent( IEvent* p_event )
{
	EventType type = p_event->type();
	switch (type)
	{
	case EVENT_GET_WINDOW_HANDLE:
		static_cast<Event_GetWindowHandle*>(p_event)->handle = winId();
		break;
	default:
		break;
	}
}

void RenderWidget::mousePressEvent( QMouseEvent* p_event )
{
	Qt::MouseButton button = p_event->button();
	if(button == Qt::LeftButton)
	{
		SETTINGS()->leftMousePressed = true;
	}
	SEND_EVENT(&IEvent(EVENT_SET_TOOL));


	// Inform about key press
	SEND_EVENT(&Event_MousePress(button, true));
}

void RenderWidget::mouseReleaseEvent( QMouseEvent* p_event )
{
	Qt::MouseButton button = p_event->button();
	if(button == Qt::LeftButton)
	{
		SETTINGS()->leftMousePressed = false;
	}
	SEND_EVENT(&IEvent(EVENT_SET_TOOL));

	// Inform about key press
	SEND_EVENT(&Event_MousePress(button, false));
}

void RenderWidget::resizeEvent(QResizeEvent* e)
{
	QWidget::resizeEvent(e);

	SEND_EVENT(&Event_WindowResize(width(), height()));
}

void RenderWidget::mouseMoveEvent( QMouseEvent *e )
{
	// calculate change (delta) in mouse position
	QPoint mouseAnchor = QWidget::mapToGlobal(QPoint(this->width()*0.5f,this->height()*0.5f));
	//QCursor::setPos(mouseAnchor.x(), mouseAnchor.y()); // anchor mouse again
	int dx = e->globalX() - mouseAnchor.x();
	int dy = e->globalY() - mouseAnchor.y();

	int x = e->globalX() - mouseAnchor.x();
	int y = e->globalY() - mouseAnchor.y();

	// send mouse move event to relevant observers
	SEND_EVENT(&Event_MouseMove(x, y, dx, dy));
}
