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
#include "Queue.h"
#include "TouchEvent.h"
#include "Entity.h"
#include "Scene.h"

using namespace std;
using namespace boost::tuples;
typedef tuple<Scene *, TouchEvent> TouchEventQueueItem;


class EventHandlerBase {
public:
	boost::thread _thread;
	bool _running;
	Queue<TouchEventQueueItem> *_outputQueue;
	vector<Scene *> _scenes;

	EventHandlerBase();
	virtual ~EventHandlerBase();
	void Start();
	void Stop();
	Queue<TouchEventQueueItem> *GetOutputQueue();
	void HandleTouches(touchVector_t & down, touchVector_t & up);

private:
	shout_t *_shout;
	STouchManager *_touchManager;
	void _HandleEventsForever();
	void _FilterEvent(TT_touch_state_t *obj, bool isDown);
	Scene *_GlobalCoordsToScene(float x, float y);
	void _VisualizeShoutEvent(float x, float y);

};

#endif /* EVENTHANDLERBASE_H_ */
