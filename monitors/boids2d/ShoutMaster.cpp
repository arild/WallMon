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

ShoutMaster::ShoutMaster()
{
	_outputQueue = new Queue<TouchEvent> (10000);
}

ShoutMaster::~ShoutMaster()
{
	delete _outputQueue;
}

void ShoutMaster::Start()
{
	_running = true;
	_thread = boost::thread(&ShoutMaster::_HandleShoutEventsForever, this);
}

void ShoutMaster::Stop()
{
	LOG(INFO) << "stopping ShoutMaster...";
	_running = false;
	_thread.join();
	LOG(INFO) << "ShoutMaster stopped";
}

Queue<TouchEvent> *ShoutMaster::GetOutputQueue()
{
	return _outputQueue;
}

void ShoutMaster::_HandleShoutEventsForever()
{
	shout_t *shout = shout_connect_default(kClient_type_both, "Boids2D");
	if (!shout)
		shout = shout_connect(kClient_type_both, "rocksvv.cs.uit.no", kShout_default_port,
				"Boids2D");
	if (!shout) {
		LOG(WARNING) << "no shout server available, touch will not work.";
		;
		return;
	}

	uint32_t filter[] = { kEvt_type_calibrated_touch_location };
	shout_set_event_filter(shout, sizeof(filter) / 4, filter);
	while (_running) {
		//		evt = shout_poll_event(shout);
		LOG(INFO) << "Waiting for event... ";
		shout_event_t *e = shout_wait_next_event(shout);
		LOG(INFO) << "Event received! ";
		_ParseShoutEvent(e);
	}
}

void ShoutMaster::_ParseShoutEvent(shout_event_t *event)
{
	uint32_t loc_flags;
	float x, y, radius;
	int32_t layer;
	if (parse_touch_location_event(event, &loc_flags, &x, &y, &radius, &layer) == 0) {
		//			if (evt->type == kEvt_type_calibrated_touch_location)
		//				printf("Calib: [ID: %3d] [PT: %.2f %.2f] [R: %5.2f] [Layer: %2d]\n", evt->refcon,
		//						x, y, radius, layer);
		//			else if (evt->type == kEvt_type_touch_location)
		//				printf("  Raw: [ID: %3d] [PT: %.2f %.2f] [R: %5.2f] [Layer: %2d]\n", evt->refcon,
		//						x, y, radius, layer);

		// Convert y coord: in shout 0,0 is at the top-left corner
		y = WALL_SCREEN_HEIGHT - y;

		Scene *scene = _GlobalCoordsToScene(x, y);
		if (scene == NULL) {
			//LOG_EVERY_N(INFO, 100) << "event discarded";
			LOG(INFO) << "event discarded";
			return;
		}

		TouchEvent touchEvent(scene, x, y);
		_outputQueue->Push(touchEvent);
	}
		else
		LOG(INFO) << "failed parsing raw shout event";

}

Scene *ShoutMaster::_GlobalCoordsToScene(float x, float y)
{
	BOOST_FOREACH(Scene *s, Scene::scenes)
	{
		if (x >= s->x && x <= s->x + s->w && y >= s->y && y <= s->y + s->h)
			return s;
	}
	return NULL;
}

