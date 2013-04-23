#pragma once

class IEvent;

// Interface needed for receiving Events
class IObserver
{
public:
	IObserver(){}
	~IObserver(){}
	virtual void onEvent(IEvent* e) = 0;
};