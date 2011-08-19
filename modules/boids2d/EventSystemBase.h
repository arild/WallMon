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


class EventSystemBase {
public:
	Queue<TouchEvent> *eventQueue;

	// Methods intended for external use (API)
	EventSystemBase();
	virtual ~EventSystemBase();
	void SetWallView(WallView *wallView);
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
	void FilterAndRouteEvent(TT_touch_state_t *obj, bool isDown);
private:
	boost::thread _thread;
	bool _running;
	WallView *_wallView;

	void _HandleEventsForever();
	Scene *_GlobalCoordsToScene(float x, float y);
	void _VisualizeEvent(float x, float y);

};

#endif /* EVENTHANDLERBASE_H_ */
