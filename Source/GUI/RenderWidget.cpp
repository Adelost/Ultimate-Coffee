#include "stdafx.h"
#include "RenderWidget.h"
#include <Core/DataMapper.h>
#include <Core/Data_Camera.h>
#include <Core/Command_CreateEntity.h>
#include <QApplication.h>
#include <QRubberBand>
#include "Window.h"
#include "ui_MainWindow.h"

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
	

	multiSelect = new Tool_MultiSelect(this);
}

RenderWidget::~RenderWidget()
{
}

void RenderWidget::onEvent( Event* p_event )
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
			case Event_SetCursor::SceneCursor:
				{
					QString path = ICON_PATH;
					path += "Cursors/scene";
					QPixmap pixmap(path);
					int hotspotX = 0;
					int hotspotY = 0;
					QCursor cursor(pixmap, hotspotX, hotspotY);
					QWidget::setCursor(cursor);	
				}
				break;
			case Event_SetCursor::SceneCursor_Pointer:
				{
					QString path = ICON_PATH;
					path += "Cursors/scene_pointer";
					QPixmap pixmap(path);
					int hotspotX = 0;
					int hotspotY = 0;
					QCursor cursor(pixmap, hotspotX, hotspotY);
					QWidget::setCursor(cursor);	
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
	case EVENT_START_MULTISELECT:
		{
			Int2 position = static_cast<Event_SetCursorPosition*>(p_event)->position;
			QPoint globalPosition = QWidget::mapToGlobal(QPoint(position.x, position.y));
			QCursor::setPos(globalPosition);
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

	// Resize cameras
	// NOTE: Don't know if this should be here,
	// but in the meantime...
	DataMapper<Data::Camera> map_camera;
	while(map_camera.hasNext())
	{
		Data::Camera* d_camera = map_camera.next();
		float aspectRatio =  static_cast<float>(width)/height;
		d_camera->setLens(0.25f*Math::Pi, aspectRatio, 0.1f, 5000.0f);
	}

	// Notify rest of code
	SEND_EVENT(&Event_WindowResize(width, height));
}

void RenderWidget::mouseMoveEvent( QMouseEvent* e )
{
	// Save mouse position
	SETTINGS()->lastMousePosition.x = e->pos().x();
	SETTINGS()->lastMousePosition.y = e->pos().y();

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

	
	// ELESE: Adjust camera rotation
	if(SETTINGS()->button.mouse_right)
	{
		QCursor::setPos(mouseAnchor.x(), mouseAnchor.y()); // anchor mouse again
		mousePrev = mouseAnchor;

		// Set 1 pixel = 0.25 degrees
		float x = XMConvertToRadians(0.20f*(float)dx);
		float y = XMConvertToRadians(0.20f*(float)dy);

		// Rotate camera
		d_camera->rotateX(y);
		d_camera->rotateY(x);
		d_camera->updateViewMatrix(d_transform->position);
	}

	if(SETTINGS()->button.mouse_middle)
	{
		float strafe = 0.0f;
		float ascend = 0.0f;

		strafe = -0.02f*dx*d_camera->scale();
		ascend = 0.02f*dy*d_camera->scale();

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
		break;
	case Qt::Key_Control:
		SETTINGS()->button.key_ctrl = state;
		break;
	case Qt::Key_Alt:
		SETTINGS()->button.key_alt = state;
		break;
	case Qt::Key_Space:
		{
			SETTINGS()->button.key_space = state;
			// Zoom to shortcut
			if(state && SETTINGS()->button.key_ctrl)
			{
				if(Data::Selected::lastSelected.isValid())
				{
					Data::ZoomTo::zoomTo(Data::Selected::lastSelected.asEntity());
				}
			}
		}
		break;
	case Qt::Key_Delete:
		if(state)
		{
			// Delete all selected entities
			std::vector<Command*> command_list;
			DataMapper<Data::Selected> m_selected;
			// Find name of deleted Entity
			std::string entityName;
			if(m_selected.dataCount() == 1)
			{
				entityName = m_selected.nextEntity()->name();
				m_selected.resetIndex();
			}
			while(m_selected.hasNext())
			{
				Entity* e = m_selected.nextEntity();

				if(e->type() == Enum::Entity_Camera)
				{
					MESSAGEBOX("Take it easy son. We only got one camera here. It's expensive");
				}
				else
				{
					command_list.push_back(new Command_CreateEntity(e, false));
					e->removeEntity();
				}
				
			}
			if(command_list.size() > 0)
			{
				//check. If an entity has a name that needs to be saved to file, put it in the data struct of the command (Henrik, 2013-05-18, 14.34)
				//if(count == 1)
				//	command_list.back()->setName("Removed " + entityName);
				//else
				//	command_list.back()->setName("Removed Entity");

				SEND_EVENT(&Event_AddToCommandHistory(&command_list, false));
			}
			
			
		}
		break;
	default:
		break;
	}

	// Set tool hotkey
	if(state)
	{
		switch(key)
		{
		case Qt::Key_1:
			SETTINGS()->setSelectedTool(Enum::Tool_Translate);
			break;
		case Qt::Key_2:
			SETTINGS()->setSelectedTool(Enum::Tool_Rotate);
			break;
		case Qt::Key_3:
			SETTINGS()->setSelectedTool(Enum::Tool_Scale);
			break;
		case Qt::Key_4:
			SETTINGS()->setSelectedTool(Enum::Tool_Geometry);
			break;
		case Qt::Key_5:
			SETTINGS()->setSelectedTool(Enum::Tool_Entity);
			break;
		default:
			break;
		}
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

	// Set Ctrl and Shift to allow  Ctrl-click
	// without having Window focus
	SETTINGS()->button.key_ctrl = (QApplication::keyboardModifiers() & Qt::ControlModifier);
	SETTINGS()->button.key_shift = (QApplication::keyboardModifiers() & Qt::ShiftModifier);
	SETTINGS()->button.key_alt = (QApplication::keyboardModifiers() & Qt::AltModifier);

	// Inform about key press
	QPoint pos = p_event->pos();
	SEND_EVENT(&Event_MousePress(pos.x(), pos.y(), button, state));

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

	// HACK: Place Entities if EntityTool is selected
	if(p_pressed && button == Qt::LeftButton && SETTINGS()->selectedTool() == Enum::Tool_Geometry)
	{
		// Compute picking ray to place Entities onto
		Float2 windowSize(SETTINGS()->windowSize.x, SETTINGS()->windowSize.y);
		Ray r;

		Entity* cam = CAMERA_ENTITY().asEntity();
		Data::Transform* d_transform = cam->fetchData<Data::Transform>();
		Data::Camera* d_camera = cam->fetchData<Data::Camera>();
		d_camera->getPickingRay(Float2(pos.x(), pos.y()), windowSize, &r);

		// Translate ray to world space
		Matrix mat_world = d_transform->toRotPosMatrix();
		r.position = Vector3::Transform(r.position, mat_world);
		r.direction = Vector3::TransformNormal(r.direction, mat_world);

		// Calculate entity position
		{
			Entity* entity = WORLD()->factory_entity()->createEntity(Enum::Entity_Mesh);
			Data::Transform* d_transform = entity->fetchData<Data::Transform>();
			Vector3 pos = r.position + r.direction * 15.0f * d_camera->scale();
			d_transform->position = pos;
			d_transform->scale = d_transform->scale * d_camera->scale();

			// Add to history
			SEND_EVENT(&Event_AddToCommandHistory(new Command_CreateEntity(entity), false));
		}
	}
}

void RenderWidget::wheelEvent( QWheelEvent* e )
{
	int scroll = 0;
	if(e->delta() > 0)
		scroll = 1;
	if(e->delta() < 0)
		scroll = -1;


	// Adjust camera speed

	// HACK: This is a hack, and you know it
	static float value = 0.0005; // Magic number makes speed 1
	value += (scroll) * 1.0f;

	// Restore to normal if key is pressed
	if(SETTINGS()->button.key_left)
		value = 0.0005;

	float speed = Math::powerOf(2.0f, value);

	// Set scale
	Window::instance()->ui()->statusBar->showMessage("Camera scale: " + QString::number(1/speed)+":1", 1500);

	// Fetch camera
	Entity* entity_camera = CAMERA_ENTITY().asEntity();
	Data::Transform* d_transform = entity_camera->fetchData<Data::Transform>();
	Data::Camera* d_camera = entity_camera->fetchData<Data::Camera>();

	d_camera->setScale(speed);

}

Tool_MultiSelect::Tool_MultiSelect( QWidget* parent ) : QObject(parent)
{
	used = false;
	rubberBand = new QRubberBand(QRubberBand::Rectangle, parent);
	SUBSCRIBE_TO_EVENT(this, EVENT_START_MULTISELECT);
	SUBSCRIBE_TO_EVENT(this, EVENT_MOUSE_MOVE);
	SUBSCRIBE_TO_EVENT(this, EVENT_MOUSE_PRESS);
}

void Tool_MultiSelect::onEvent( Event* event )
{
	EventType type = event->type();
	switch (type)
	{
	case EVENT_START_MULTISELECT:
		{
			origin = SETTINGS()->lastMousePosition;
			rubberBand->setGeometry(QRect(QPoint(origin.x, origin.y), QSize()));
			rubberBand->show();
			used = true;
		}
		break;
	case EVENT_MOUSE_MOVE:
		{
			if(used)
			{
				Int2 pos = SETTINGS()->lastMousePosition;
				QRect size = QRect(QPoint(pos.x, pos.y), QPoint(origin.x, origin.y)).normalized();
				rubberBand->setGeometry(size);
			}
		}
		break;
	case EVENT_MOUSE_PRESS:
		{
			// Stop "using" the tool if user releases LeftMouseButton
			int limit = 10;
			if(used)
			{
				Event_MousePress* e = static_cast<Event_MousePress*>(event);
				if(e->keyEnum == Enum::QtKeyPress_MouseLeft && !e->isPressed)
				{
					used = false;
					rubberBand->hide();

					// Perform selection
					if(rubberBand->width() > limit || rubberBand->height() > limit)
					{
						// Fetch camera
						Entity* entity_camera = CAMERA_ENTITY().asEntity();
						Data::Transform* d_transform = entity_camera->fetchData<Data::Transform>();
						Data::Camera* d_camera = entity_camera->fetchData<Data::Camera>();

						// Compute sub frustum
						FloatRectangle window(Float2(0, 0), Float2(SETTINGS()->windowSize.x, SETTINGS()->windowSize.y));
						FloatRectangle sub_window(Float2(rubberBand->x(), rubberBand->y()), Float2(rubberBand->width(), rubberBand->height()));
						// Height/Width needs to be larger than 0 to construct frustum
						if(sub_window.size.x < 1)
							sub_window.size.x = 1;
						if(sub_window.size.y < 1)
							sub_window.size.y = 1;
						BoundingFrustum subFrustum = d_camera->getSubFrustum(window, sub_window);

						// Find intersected Entities
						std::vector<Entity*> entity_list;
						Data::Bounding::intersect(subFrustum, &entity_list);

						// If Ctrl is pressed Entities are removed
						// from selection, otherwise they are added
						if(SETTINGS()->button.key_ctrl)
						{
							for(int i=0; i<entity_list.size(); i++)
							{
								Data::Selected::unselect(entity_list[i]);
							}
						}
						else
						{
							bool onlyOne = entity_list.size() == 1;
							for(int i=0; i<entity_list.size(); i++)
							{
								if(onlyOne || entity_list[i]->type() != Enum::Entity_DirLight && entity_list[i]->type() != Enum::Entity_Pointlight)
									Data::Selected::select(entity_list[i]);
							}
						}
						

						// Inform about selection
						SEND_EVENT(&Event(EVENT_ENTITY_SELECTION));
					}
				}
			}
		}
		break;
	default:
		break;
	}
}
