/*
 * ShoutEventSystem.cpp
 *
 *  Created on: Jun 10, 2011
 *      Author: arild
 */

#include "ShoutEventSystem.h"
#include "touchmanager/STouchManager.h"
#include <glog/logging.h>

typedef void (ShoutEventSystem::*HandleTouchesPtr)(touchVector_t &down, touchVector_t &up);

ShoutEventSystem::ShoutEventSystem()
{
}

ShoutEventSystem::~ShoutEventSystem()
{
	shout_disconnect(_shout);
}

bool ShoutEventSystem::InitEventSystem()
{
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
	shout_event_t *event = shout_wait_next_event(_shout);
	FilterAndRouteEvent(event);
}




