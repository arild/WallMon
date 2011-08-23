/*
 * EventHandlerBase.h
 *
 *  Created on: May 17, 2011
 *      Author: arild
 */

#ifndef EVENTHANDLERBASE_H_
#define EVENTHANDLERBASE_H_

#include <vector>
#include <boost/thread.hpp>
#include <boost/tuple/tuple.hpp>
#include <shout/event-types/touch-events.h>
#include "touchmanager/STouchManager.h"
#include "WallView.h"
#include "Queue.h"
#include "TouchEvent.h"
#include "Entity.h"
#include "Scene.h"

using namespace std;
using namespace boost::tuples;

typedef tuple<Entity *, TouchEvent> EventQueueItem;

class EventSystemBase {
public:
	Queue<EventQueueItem> *eventQueue;

	// Methods intended for external use (API)
	EventSystemBase();
	virtual ~EventSystemBase();
	void SetWallView(WallView *wallView);
	void AddScene(Scene *scene);
	void Start();
	void Stop();

	// Methods intended to be implemented via inheritance

	// Called by the thread executing the main thread. It is intended to used
	// where the main thread has to do the event fetching, such as, when the
	// main thread holds all the SDL and/or OpenGL context
	virtual void PollEvents() = 0;
	virtual bool InitEventSystem() = 0;
	virtual void WaitAndHandleNextEvent() = 0;

	// Methods intended to be used by child class
	void FilterAndRouteEvent(TouchEvent &event);
private:
	boost::thread _thread;
	bool _running;
	WallView *_wallView;
	vector<Scene *> _scenes;
	void _HandleEventsForever();
};

#endif /* EVENTHANDLERBASE_H_ */
