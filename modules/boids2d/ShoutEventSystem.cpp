/*
 * ShoutEventSystem.cpp
 *
 *  Created on: Jun 10, 2011
 *      Author: arild
 */

#include "ShoutEventSystem.h"
#include "touchmanager/STouchManager.h"

typedef void (ShoutEventSystem::*HandleTouchesPtr)(touchVector_t &down, touchVector_t &up);

ShoutEventSystem::ShoutEventSystem()
{
}

ShoutEventSystem::~ShoutEventSystem()
{
	shout_disconnect(_shout);
	delete _touchManager;
}

bool ShoutEventSystem::InitEventSystem()
{
	HandleTouchesPtr handle = &ShoutEventSystem::HandleTouches;
	touchManagerCallback_t callback = *(touchManagerCallback_t*) &handle;
	_touchManager = new STouchManager(this, callback);

	_shout = shout_connect_default(kClient_type_both, "Boids2D");
	if (!_shout)
		_shout = shout_connect(kClient_type_both, "rocksvv.cs.uit.no", kShout_default_port,"Boids2D");
	if (!_shout)
		return false;

	uint32_t filter[] = { kEvt_type_calibrated_touch_location, kEvt_type_touch_remove };
	shout_set_event_filter(_shout, sizeof(filter) / 4, filter);
	return true;
}

void ShoutEventSystem::PollEvents()
{
}

void ShoutEventSystem::WaitAndHandleNextEvent()
{
	TouchEvent event;
	shout_event_t *shoutEvent = shout_wait_next_event(_shout);
	uint32_t locFlags, senderID;
	float x, y, radius;
	if (parse_touch_location_event_v2(shoutEvent, &locFlags, &x, &y, &radius, 0, &senderID) == 0) {
		event.x = x;
		event.y = y;
		event.radius = radius;
		event.id = shoutEvent->refcon;
		event.isUp = locFlags & kTouch_evt_last_detect_flag;
		FilterAndRouteEvent(event);
	}
}

void ShoutEventSystem::HandleTouches(touchVector_t & down, touchVector_t & up)
{
//	for (int i = 0; i < down.size(); i++) {
//		TT_touch_state_t *obj = down[i];
//		obj->isUp = false;
//		if (obj->wasUpdated)
//			FilterAndRouteEvent(obj);
//	}
//	for (int i = 0; i < up.size(); i++) {
//		TT_touch_state_t *obj = up[i];
//		obj->isUp = true;
//		if (obj->wasUpdated)
//			FilterAndRouteEvent(obj);
//	}
}




