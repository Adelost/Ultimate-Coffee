#pragma once

class IObserver;
enum EventType;
class IEvent;

class DelayedEvent
{
private:
	IEvent* e;
	float delay;

public:
	DelayedEvent(IEvent* e, float delay);

	// Returns TRUE if the delay timer expires or has expired
	bool isReady(float delta);

	IEvent* getEvent();
};

// Redirect events to each appropriate IObserver.

class EventManager
{
private:
	std::vector<std::vector<IObserver*>>* subscribers;	
	std::vector<std::vector<IEvent*>>* queues;			//! Used to queue Events if queue feature is used
	std::vector<std::vector<DelayedEvent>>* delayedQueues;	//! Same function as queues, except message is delayed from sending until at specified time interval is sent

	EventManager();
	~EventManager();

public:
	static EventManager* getInstance();

	void addObserver(IObserver* o, EventType type);

	/** 
	Adds observer to every event queue.
	Observers should not waste traffic by
	subscribing to events they are not using.
	Redirect events to each appropriate IObserver.
	*/
	void addObserverToAll(IObserver* o);

	/** 
	Removes all instances of Observer.
	*/
	void removeObserver(IObserver* observer);

	/** 
	Removes instance of Observer from a specific event.
	NOTE: Order is not important, uses "swap trick" 
	to swap "last element" with "removed"
	This gives O(n) performance for remove instead of O(n^2).
	*/
	void removeObserver(IObserver* observer, EventType type);

	/** 
	Sends event to all relevant observers.
	*/
	void sendEvent(IEvent* e);

	/** 
	Queue an event so it can be sent later by using flushEventQueue.
	*/
	void queueEvent(IEvent* e);

	/** 
	Sends queued Events
	*/
	void flushQueuedEvents(EventType type);

	/** 
	Get pointer to the internal vector holding the queued Events
	Must be Flushed afterwards to make sure events are deleted.
	*/
	std::vector<IEvent*>* getPointerToQueuedEvents(EventType type);

	/** 
	Removes all queued messages without sending them,
	prevents memory leaks if not all messages is sent.
	*/
	void cleanAllQueues();

	/** 
	Performs necessary per-frame updating of some sub-parts of EventManager.
	*/
	void update(float delta);

	/** 
	Decrement timer on delayed events and sends those that expires.
	*/
	void sendExpiredDelayedEvents(float delta);

	/** 
	Post an event that will be sent when the delay (in seconds) expires
	*/
	void postDelayedEvent(IEvent* e, float delay);
};



// Sends Event to relevant listeners.
#define SEND_EVENT(EVENT_POINTER)									\
	EventManager::getInstance()->sendEvent(EVENT_POINTER);

// Queue an event so it can be sent later by using FLUSH MACRO.
// Memory deletion is handled by EventManager. 
#define QUEUE_EVENT(EVENT_POINTER)									\
	EventManager::getInstance()->queueEvent(EVENT_POINTER);

// Sends all queued events to relevant listeners.
#define FLUSH_QUEUED_EVENTS(EVENT_TYPE)								\
	EventManager::getInstance()->flushQueuedEvents(EVENT_TYPE);

// Sends all queued events to relevant listeners.
#define POST_DELAYED_EVENT(EVENT_POINTER, DELAY)					\
	EventManager::getInstance()->postDelayedEvent(EVENT_POINTER, DELAY);

// Subscribes a IObserver to events of EventType.
#define SUBSCRIBE_TO_EVENT(SUBSCRIBER, EVENT_TYPE)					\
	EventManager::getInstance()->addObserver(SUBSCRIBER, EVENT_TYPE);

// Unsubscribes a IObserver to events of EventType.
#define UNSUBSCRIBE_TO_EVENT(SUBSCRIBER, EVENT_TYPE)				\
	EventManager::getInstance()->removeObserver(SUBSCRIBER, EVENT_TYPE);

// Unsubscribes a IObserver to all events.
#define UNSUBSCRIBE_TO_ALL_EVENT(SUBSCRIBER)							\
	EventManager::getInstance()->removeObserver(SUBSCRIBER);

// Fetches a owners of a specific Attribute from AttributeManager
#define MESSAGEBOX(MESSAGE)									\
{																	\
	Event_ShowMessageBox IF_YOU_CAN_READ_THIS_TRY_INCLUDING_EVENT_H_FOUND_IN_CORE (MESSAGE);							\
	EventManager::getInstance()->sendEvent(&IF_YOU_CAN_READ_THIS_TRY_INCLUDING_EVENT_H_FOUND_IN_CORE);					\
}
