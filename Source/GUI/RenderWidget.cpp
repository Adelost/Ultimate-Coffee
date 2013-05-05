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
	setMouseTracking(true);
	//setStyleSheet("background-color: rgba(0, 0, 0, 255);");

	SUBSCRIBE_TO_EVENT(this, EVENT_GET_WINDOW_HANDLE);
	SUBSCRIBE_TO_EVENT(this, EVENT_SET_CURSOR_POSITION);
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
	case EVENT_SET_CURSOR_POSITION:
		{
			Int2 position = static_cast<Event_SetCursorPosition*>(p_event)->position;
			QCursor::setPos(position.x, position.y);
		}
		break;
	default:
		break;
	}
}

void RenderWidget::mousePressEvent( QMouseEvent* p_event )
{
	setFocus();

	Qt::MouseButton button = p_event->button();
	if(button == Qt::LeftButton)
		SETTINGS()->button.mouse_left = true;
	if(button == Qt::RightButton)
		SETTINGS()->button.mouse_right = true;
	if(button == Qt::MiddleButton)
		SETTINGS()->button.mouse_middle = true;

	QPoint pos = p_event->pos();
	SEND_EVENT(&IEvent(EVENT_SET_TOOL));


	// Inform about key press
	SEND_EVENT(&Event_MousePress(pos.x(), pos.y(), button, true));
}

void RenderWidget::mouseReleaseEvent( QMouseEvent* p_event )
{
	Qt::MouseButton button = p_event->button();
	QPoint pos = p_event->pos();
	if(button == Qt::LeftButton)
		SETTINGS()->button.mouse_left = false;
	if(button == Qt::RightButton)
		SETTINGS()->button.mouse_right = false;
	if(button == Qt::MiddleButton)
		SETTINGS()->button.mouse_middle = false;

	SEND_EVENT(&IEvent(EVENT_SET_TOOL));

	// Inform about key press
	SEND_EVENT(&Event_MousePress(pos.x(), pos.y(), button, false));
}

void RenderWidget::resizeEvent(QResizeEvent* e)
{
	QWidget::resizeEvent(e);
	
	int width = this->width();
	int height = this->height();
	SETTINGS()->windowSize = Int2(width, height);

	SEND_EVENT(&Event_WindowResize(width, height));
}

void RenderWidget::mouseMoveEvent( QMouseEvent* e )
{
	QPoint mouseAnchor = mapToGlobal(QPoint(width()*0.5f, height()*0.5f));
	static QPoint mousePrev = e->globalPos();
	int x = e->pos().x();
	int y = e->pos().y();
	int dx = e->globalX() - mousePrev.x();
	int dy = e->globalY() - mousePrev.y();
	mousePrev = e->globalPos();


	// send mouse move event to relevant observers
	SEND_EVENT(&Event_MouseMove(x, y, dx, dy));

	Entity entity_camera = CAMERA_ENTITY();
	Data::Transform* d_transform = entity_camera.fetchData<Data::Transform>();
	Data::Camera* d_camera = entity_camera.fetchData<Data::Camera>();

	if(SETTINGS()->button.mouse_right)
	{
		QCursor::setPos(mouseAnchor.x(), mouseAnchor.y()); // anchor mouse again
		//SEND_EVENT(&Event_SetCursorPosition(Int2(mouseAnchor.x(), mouseAnchor.y())))
		mousePrev = mouseAnchor;

		// Set 1 pixel = 0.25 degrees
		float x = XMConvertToRadians(0.20f*(float)dx);
		float y = XMConvertToRadians(0.20f*(float)dy);

		// Rotate camera
		d_camera->rotateZ(y);
		d_camera->rotateY(x);
		d_camera->updateViewMatrix(d_transform->position);
	}

	if(SETTINGS()->button.mouse_middle)
	{
		//QCursor::setPos(mousePrev.x(), mousePrev.y()); // anchor mouse again
		//mousePrev = mouseAnchor;

		
		float strafe = 0.0f;
		float ascend = 0.0f;

		strafe = -0.02f*dx;
		ascend = 0.02f*dy;

		// Rotate camera
		d_camera->strafe(d_transform->position, strafe);
		d_camera->ascend(d_transform->position, ascend);
		d_camera->updateViewMatrix(d_transform->position);
	}
}


void RenderWidget::keyPressEvent( QKeyEvent* e )
{
	setKeyState(e, true);
}

void RenderWidget::keyReleaseEvent( QKeyEvent* e )
{
	setKeyState(e, false);
}

void RenderWidget::setKeyState( QKeyEvent* p_event, bool p_pressed )
{
	Qt::Key key = static_cast<Qt::Key>(p_event->key());
	bool state = p_pressed;

	switch(key)
	{
	case Qt::Key_W:
		SETTINGS()->button.key_up = state;
		break;
	case Qt::Key_S:
		SETTINGS()->button.key_down = state;
		break;
	case Qt::Key_A:
		SETTINGS()->button.key_left = state;
		break;
	case Qt::Key_D:
		SETTINGS()->button.key_right = state;
		break;
	default:
		break;
	}
}