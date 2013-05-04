#include "stdafx.h"
#include "RenderWidget.h"
#include <Core/Events.h>
#include <Core/World.h>
#include <Core/Data.h>

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
	QPoint pos = p_event->pos();
	if(button == Qt::LeftButton)
	{
		SETTINGS()->leftMousePressed = true;
	}
	SEND_EVENT(&IEvent(EVENT_SET_TOOL));


	// Inform about key press
	SEND_EVENT(&Event_MousePress(pos.x(), pos.y(), button, true));
}

void RenderWidget::mouseReleaseEvent( QMouseEvent* p_event )
{
	Qt::MouseButton button = p_event->button();
	QPoint pos = p_event->pos();
	if(button == Qt::LeftButton)
	{
		SETTINGS()->leftMousePressed = false;
	}
	SEND_EVENT(&IEvent(EVENT_SET_TOOL));

	// Inform about key press
	SEND_EVENT(&Event_MousePress(pos.x(), pos.y(), button, false));
}

void RenderWidget::resizeEvent(QResizeEvent* e)
{
	QWidget::resizeEvent(e);
	
	int width = this->width();
	int height = this->height();

	SEND_EVENT(&Event_WindowResize(width, height));
}

void RenderWidget::mouseMoveEvent( QMouseEvent* e )
{
	static QPoint prevMousePos = e->pos();
	QPoint pos = e->pos();

	int dx = pos.x() - prevMousePos.x();
	int dy = pos.y() - prevMousePos.y();
	
	prevMousePos = pos;

	int x = pos.x();
	int y = pos.y();

	// send mouse move event to relevant observers
	SEND_EVENT(&Event_MouseMove(x, y, dx, dy));

	// Update camera
	Entity entity_camera = CAMERA_ENTITY();
	Data::Transform* d_transform = entity_camera.fetchData<Data::Transform>();
	Data::Camera* d_camera = entity_camera.fetchData<Data::Camera>();

	{
		// Set 1 pixel = 0.25 degrees
		float x = XMConvertToRadians(0.20f*(float)dx);
		float y = XMConvertToRadians(0.20f*(float)dy);

		// Rotate camera
		d_camera->rotateZ(y);
		d_camera->rotateY(x);
		d_camera->updateViewMatrix(d_transform->position);
	}
}
