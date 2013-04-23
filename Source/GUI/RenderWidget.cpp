#include <RenderWidget.h>

#include <Core/EventManager.h>
#include <Core/Events.h>


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

void RenderWidget::onEvent(IEvent* e)
{
	EventType type = e->type();
	switch (type) 
	{
	case EVENT_GET_WINDOW_HANDLE:
		static_cast<Event_GetWindowHandle*>(e)->handle = winId();
		break;
	default:
		break;
	}
}

