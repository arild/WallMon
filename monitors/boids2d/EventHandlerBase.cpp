/*
 * EventHandlerBase.cpp
 *
 *  Created on: May 17, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include <boost/foreach.hpp>
#include <shout/shout.h>
#include "WallView.h"
#include "Config.h"
#include "EventHandlerBase.h"


typedef void (EventHandlerBase::*HandleTouchesPtr)(touchVector_t &down, touchVector_t &up);

EventHandlerBase::EventHandlerBase()
{
	_shout = shout_connect_default(kClient_type_both, "Boids2D");
	if (!_shout)
		_shout = shout_connect(kClient_type_both, "rocksvv.cs.uit.no", kShout_default_port,
				"Boids2D");
	if (!_shout) {
		LOG(WARNING) << "no shout server available, touch will not work.";
		;
		return;
	}

	uint32_t filter[] = { kEvt_type_calibrated_touch_location, kEvt_type_touch_remove };
	shout_set_event_filter(_shout, sizeof(filter) / 4, filter);

	_outputQueue = new Queue<TouchEventQueueItem> (10000);
	HandleTouchesPtr handle = &EventHandlerBase::HandleTouches;
	touchManagerCallback_t callback = *(touchManagerCallback_t*) &handle;
	_touchManager = new STouchManager(this, callback);
}

EventHandlerBase::~EventHandlerBase()
{
	delete _outputQueue;
}

void EventHandlerBase::Start()
{
	_running = true;
	_thread = boost::thread(&EventHandlerBase::_HandleEventsForever, this);
}

void EventHandlerBase::Stop()
{
	LOG(INFO) << "stopping EventHandlerBase...";
	_running = false;
	_thread.join();
	LOG(INFO) << "EventHandlerBase stopped";
}

Queue<TouchEventQueueItem> *EventHandlerBase::GetOutputQueue()
{
	return _outputQueue;
}

void EventHandlerBase::HandleTouches(touchVector_t & down, touchVector_t & up)
{
	for (int i = 0; i < down.size(); i++) {
		TT_touch_state_t *obj = down[i];
		if (obj->wasUpdated)
			_FilterEvent(obj, true);
	}
	for (int i = 0; i < up.size(); i++) {
		TT_touch_state_t *obj = up[i];
		if (obj->wasUpdated)
			_FilterEvent(obj, false);
	}
}

void EventHandlerBase::_HandleEventsForever()
{
	while (_running) {
		shout_event_t *event = shout_wait_next_event(_shout);
		_touchManager->handleEvent(event);
	}
}

void EventHandlerBase::_FilterEvent(TT_touch_state_t *obj, bool isDown)
{
	float x = (float)obj->loc.x;
	float y = (float)obj->loc.y;

	// Convert y coordinate: 0,0 assumed to be in the top-left corner of input
	y = WALL_SCREEN_HEIGHT - y;

	LOG(INFO) << "Checking grid";
	WallView w(2, 1, 3, 3);
	if (w.IsCordsWithin(x, y) == false)
		return;

	LOG(INFO) << "Event: x=" << x << " | y=" << y;

	// Transform the global display wall coordinates from shout into global
	// coordinates within the application, which uses the same coordinate system,
	// however, might be (significantly) smaller in physical screen size (.e.g 2x2 tiles)
	w.GlobalToGridCoords(&x, &y);

	TouchEvent event;
	event.isDown = isDown;
	event.realX = x;
	event.realY = y;
	LOG(INFO) << "Maps to: x=" << x << " | y=" << y;
	event.timestampSec = obj->time;
	event.timestampPreviousSec = (float)obj->lastUpdated;

	Scene *scene = Scene::TestForSceneHit(event.realX, event.realY);
	if (scene == NULL)
		// Coordinates not within any scene
		return;
	TouchEventQueueItem item;
	event.visualizeOnly = true;
	item = make_tuple((Scene *)NULL, event);
	_outputQueue->Push(item);
	event.visualizeOnly = false;

	LOG(INFO) << "Scene Hit: x=" << scene->x << " | y=" << scene->y;

	// Transform global coordinates within the application to match coordinate
	// system used within the relevant scene, which could be anything
	scene->RealToVirtualCoords(event.realX, event.realY, &event.x, &event.y);

	LOG(INFO) << "Maps to virt: x=" << event.x << " | y=" << event.y;

	item = make_tuple(scene, event);
	_outputQueue->Push(item);
}

