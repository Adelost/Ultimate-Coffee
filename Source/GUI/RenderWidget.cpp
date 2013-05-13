#include "stdafx.h"
#include "RenderWidget.h"
#include <Core/DataMapper.h>
#include <QApplication.h>

RenderWidget::RenderWidget( QWidget* parent ) : QWidget(parent)
{
	// make widget non-transparent & draw directly onto screen
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_PaintOnScreen);
	setMouseTracking(true);
	//setStyleSheet("background-color: rgba(0, 0, 0, 255);");

	SUBSCRIBE_TO_EVENT(this, EVENT_GET_WINDOW_HANDLE);
	SUBSCRIBE_TO_EVENT(this, EVENT_SET_CURSOR_POSITION);
	SUBSCRIBE_TO_EVENT(this, EVENT_SET_CURSOR);
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
			QPoint globalPosition = QWidget::mapToGlobal(QPoint(position.x, position.y));
			QCursor::setPos(globalPosition);
		}
		break;
	case EVENT_SET_CURSOR:
		{
			Event_SetCursor::CursorShape cursorShape = static_cast<Event_SetCursor*>(p_event)->cursorShape;
			switch (cursorShape)
			{
			case Event_SetCursor::HiddenCursor:
				{
					setCursor(Qt::BlankCursor);	
				}
				break;
			case Event_SetCursor::NormalCursor:
				{
					unsetCursor();	
				}
				break;
			default:
				{
					Qt::CursorShape cursor = static_cast<Qt::CursorShape>(cursorShape);
					QWidget::setCursor(cursor);	
				}
				break;
			}
		}
		break;
	default:
		break;
	}
}

void RenderWidget::mousePressEvent( QMouseEvent* e )
{
	setFocus();
	setMouseState(e, true);

	//// HACK: Select entity
	//QPoint pos = e->pos();
	//if(e->button() == Qt::LeftButton)
	//{
	//	Entity* entity_camera = CAMERA_ENTITY().asEntity();
	//	Data::Transform* d_transform = entity_camera->fetchData<Data::Transform>();
	//	Data::Camera* d_camera = entity_camera->fetchData<Data::Camera>();

	//	// Compute picking ray
	//	Vector2 windowSize(SETTINGS()->windowSize.x, SETTINGS()->windowSize.y);
	//	Ray r;
	//	d_camera->getPickingRay(Vector2(pos.x(), pos.y()), windowSize, &r);

	//	// Translate ray to world space
	//	Matrix mat_world = d_transform->toWorldMatrix();
	//	r.position = Vector3::Transform(r.position, mat_world);
	//	r.direction = Vector3::TransformNormal(r.direction, mat_world);
	//	DEBUGPRINT("");
	//	DEBUGPRINT("RAY:\n pos "+ Converter::FloatToStr(r.position.x) +","+ Converter::FloatToStr(r.position.y) +","+ Converter::FloatToStr(r.position.z) +"\n dir "+ Converter::FloatToStr(r.direction.x) +","+ Converter::FloatToStr(r.direction.y) +","+ Converter::FloatToStr(r.direction.z) +"");
	//	DEBUGPRINT("");

	//	// Find intersected Entity
	//	Entity* e = Data::Bounding::intersect(r);
	//	if(e)
	//	{
	//		// If Ctrl is pressed, Entity will be added to selection,
	//		// otherwise previous selection will be cleared
	//		ButtonState* buttonState = &SETTINGS()->button;
	//		if(SETTINGS()->button.key_ctrl)
	//		{
	//			if(e->fetchData<Data::Selected>() == nullptr)
	//			{
	//				e->addData(Data::Selected());
	//			}	
	//			else
	//			{
	//				e->removeData<Data::Selected>();
	//			}
	//		}
	//		else
	//		{
	//			Data::Selected::clearSelection();

	//			e->addData(Data::Selected());
	//		}

	//		Data::Selected::lastSelected = e->asPointer();
	//		DEBUGPRINT("CLICKED:");
	//		DEBUGPRINT(" Entity: " + Converter::IntToStr(e->id()));
	//	}
	//	else
	//	{
	//		if(!SETTINGS()->button.key_ctrl)
	//		{
	//			Data::Selected::clearSelection();
	//		}
	//	}

	//	// Debug selection
	//	DataMapper<Data::Selected> map_selected;
	//	DEBUGPRINT("SELECTED: " + Converter::IntToStr(map_selected.dataCount()));
	//	while(map_selected.hasNext())
	//	{
	//		Entity* e = map_selected.nextEntity();
	//		DEBUGPRINT(" Entity: " + Converter::IntToStr(e->id()));
	//	}


	//	// Inform about selection
	//	SEND_EVENT(&IEvent(EVENT_ENTITY_SELECTION));
	//}
}

void RenderWidget::mouseReleaseEvent( QMouseEvent* p_event )
{
	setMouseState(p_event, false);
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

	// Send mouse move event to relevant observers
	SEND_EVENT(&Event_MouseMove(x, y, dx, dy));

	Entity* entity_camera = CAMERA_ENTITY().asEntity();
	Data::Transform* d_transform = entity_camera->fetchData<Data::Transform>();
	Data::Camera* d_camera = entity_camera->fetchData<Data::Camera>();

	if(SETTINGS()->button.mouse_right)
	{
		QCursor::setPos(mouseAnchor.x(), mouseAnchor.y()); // anchor mouse again
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
	case Qt::Key_Shift:
		SETTINGS()->button.key_shift = state;
		if(SETTINGS()->button.key_shift)
		{
			DEBUGPRINT("Shift: On");
		}
		else
		{
			DEBUGPRINT("Shift: Off");
		}
		break;
	case Qt::Key_Control:
		SETTINGS()->button.key_ctrl = state;
		if(SETTINGS()->button.key_ctrl)
		{
			DEBUGPRINT("Ctrl: On");
		}
		else
		{
			DEBUGPRINT("Ctrl: Off");
		}
		break;
	case Qt::Key_Alt:
		SETTINGS()->button.key_alt = state;
		break;
	default:
		break;
	}
}

void RenderWidget::setMouseState( QMouseEvent* p_event, bool p_pressed )
{
	Qt::MouseButton button = p_event->button();
	bool state = p_pressed;

	switch(button)
	{
	case Qt::LeftButton:
		SETTINGS()->button.mouse_left = state;
		break;
	case Qt::RightButton:
		SETTINGS()->button.mouse_right = state;
		break;
	case Qt::MiddleButton:
		SETTINGS()->button.mouse_middle = state;
		break;
	default:
		break;
	}

	// Also set Ctrl and Shift to allow 
	// Ctrl-click without having Window focus
	SETTINGS()->button.key_ctrl = (QApplication::keyboardModifiers() & Qt::ControlModifier);
	SETTINGS()->button.key_shift = (QApplication::keyboardModifiers() & Qt::ShiftModifier);
	SETTINGS()->button.key_alt = (QApplication::keyboardModifiers() & Qt::AltModifier);

	// Inform about key press
	QPoint pos = p_event->pos();
	SEND_EVENT(&Event_MousePress(pos.x(), pos.y(), button, state));


	// HACK: Set tools, should be refactored later
	SEND_EVENT(&IEvent(EVENT_SET_TOOL));

	// HACK: Hide mouse when rotating camera
	if(button == Qt::RightButton)
	{
		if(state)
		{
			SEND_EVENT(&Event_SetCursor(Event_SetCursor::HiddenCursor));
		}
		else
		{
			SEND_EVENT(&Event_SetCursor(Event_SetCursor::NormalCursor));
		}
	}
}
