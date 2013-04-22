#pragma once

#include <QWidget>

class RenderWidget : public QWidget
{
	Q_OBJECT

public:
	RenderWidget(QWidget* parent) : QWidget(parent)
	{
		// make widget non-transparent & draw directly onto screen
		setAttribute(Qt::WA_OpaquePaintEvent);
		setAttribute(Qt::WA_PaintOnScreen);
	}
	~RenderWidget()
	{
	}

	 // Overrides Qt:s own paint engine. Prevents flicker.
	QPaintEngine* paintEngine() const {return 0;}

protected:
	// should not be implemented
	void paintEvent(QPaintEvent* e){}
};