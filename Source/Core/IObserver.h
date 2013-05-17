#pragma once

class Event;

// Interface needed for receiving Events
class IObserver
{
public:
	IObserver(){}
	~IObserver(){}
	virtual void onEvent(Event* e) = 0;
};