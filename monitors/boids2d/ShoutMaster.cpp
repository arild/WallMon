/**
 * @Author Arild Nilsen
 * @date   May, 2011
 *
 * Gathers shout events and map the events to a scene registered
 * in the router object, and translate the global coordinates to
 * scene coordinates.
 */

#include <glog/logging.h>
#include <boost/foreach.hpp>
#include <shout/shout.h>
#include "ShoutMaster.h"
#include "WallView.h"
#include "Config.h"

ShoutMaster::ShoutMaster()
{
}

ShoutMaster::~ShoutMaster()
{
}

void ShoutMaster::_InitEventSystem()
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

	uint32_t filter[] = { kEvt_type_calibrated_touch_location };
	shout_set_event_filter(_shout, sizeof(filter) / 4, filter);
}

void ShoutMaster::_WaitNextEvent(float *x, float *y)
{
	uint32_t loc_flags;
	float radius;
	int32_t layer;
	shout_event_t *event = shout_wait_next_event(_shout);
	if (parse_touch_location_event(event, &loc_flags, x, y, &radius, &layer) != 0) {
		LOG(INFO) << "failed parsing raw shout event";
		return;
	}
}

void ShoutMaster::PollEvents()
{
}







