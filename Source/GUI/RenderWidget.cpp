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

	SEND_EVENT(&Event_WindowResize(width, height));
}

void RenderWidget::mouseMoveEvent( QMouseEvent* e )
{
	QPoint mouseAnchor = QWidget::mapToGlobal(QPoint(this->width()*0.5f,this->height()*0.5f));
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
		//QCursor::setPos(mouseAnchor.x(), mouseAnchor.y()); // anchor mouse again

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
		//QCursor::setPos(mouseAnchor.x(), mouseAnchor.y()); // anchor mouse again

		float delta = SETTINGS()->deltaTime * 10.0f;
		float strafe = 0.0f;
		float walk = 0.0f;

		walk -= dy*delta;
		strafe += dx*delta;

		// Rotate camera
		d_camera->strafe(d_transform->position, strafe);
		d_camera->walk(d_transform->position, walk);
		d_camera->updateViewMatrix(d_transform->position);
	}
}

void RenderWidget::keyPressEvent( QKeyEvent *e )
{
	// Update camera
	Entity entity_camera = CAMERA_ENTITY();
	Data::Transform* d_transform = entity_camera.fetchData<Data::Transform>();
	Data::Camera* d_camera = entity_camera.fetchData<Data::Camera>();

	float delta = SETTINGS()->deltaTime;
	float strafe = 0.0f;
	float walk = 0.0f;

	if(e->key() == Qt::Key_W)
	{
		walk += delta;
	}
	if(e->key() == Qt::Key_A)
	{
		strafe -= delta;
	}
	if(e->key() == Qt::Key_S)
	{
		walk -= delta;
	}
	if(e->key() == Qt::Key_D)
	{
		strafe += delta;
	}

	// Rotate camera
	d_camera->strafe(d_transform->position, strafe);
	d_camera->walk(d_transform->position, walk);
	d_camera->updateViewMatrix(d_transform->position);
}
