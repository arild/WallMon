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

void ShoutMaster::Register(Scene *scene)
{
        _scenes.push_back(scene);
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
		shout_event_t *e = shout_wait_next_event(shout);
		_ParseShoutEvent(e);
	}
}

void ShoutMaster::_ParseShoutEvent(shout_event_t *event)
{
	uint32_t loc_flags;
	float x, y, radius;
	int32_t layer;
	if (parse_touch_location_event(event, &loc_flags, &x, &y, &radius, &layer) != 0) {
		LOG(INFO) << "failed parsing raw shout event";
		return;
	}

	// Convert y coordinate: in shout 0,0 is at the top-left corner
	y = WALL_SCREEN_HEIGHT - y;

	WallView w(0, 0, 7, 4);
	if (w.IsCordsWithin(x, y) == false)
		return;

	TouchEvent e;
	e.realX = x;
	e.realY = y;
	w.GlobalToGridCoords(&x, &y);
	e.sceneX = x;
	e.sceneY = y;

	Scene *scene = _GlobalCoordsToScene(x, y);
	if (scene == NULL) {
		// Coordinates not within any scene
		//LOG_EVERY_N(INFO, 100) << "event discarded";
		LOG(INFO) << "event discarded";
		return;
	}
	e.scene = scene;
	_outputQueue->Push(e);

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



