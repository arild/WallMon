/*
 * EventHandlerBase.cpp
 *
 *  Created on: May 17, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include <boost/foreach.hpp>
#include <shout/shout.h>
#include "Config.h"
#include "EventSystemBase.h"


EventSystemBase::EventSystemBase()
{
	eventQueue = new Queue<EventQueueItem> (10000);
}

EventSystemBase::~EventSystemBase()
{
	delete eventQueue;
}

void EventSystemBase::SetWallView(WallView *wallView)
{
	_wallView = wallView;
}

void EventSystemBase::AddScene(Scene *scene)
{
	_scenes.push_back(scene);
}

void EventSystemBase::Start()
{
	if (InitEventSystem() == false) {
		LOG(WARNING) << "touch system failed initializing, touch will not work.";
		return;
	}
	_running = true;
	_thread = boost::thread(&EventSystemBase::_HandleEventsForever, this);
}

void EventSystemBase::Stop()
{
	if (_running == false)
		return;
	LOG(INFO) << "stopping EventHandlerBase...";
	_running = false;
	_thread.join();
	LOG(INFO) << "EventHandlerBase stopped";
}

void EventSystemBase::_HandleEventsForever()
{
	while (_running) {
		WaitAndHandleNextEvent();
	}
}

void EventSystemBase::FilterAndRouteEvent(TT_touch_state_t *event)
{
	float x = (float)event->loc.x;
	float y = (float)event->loc.y;

	// Convert y coordinate: 0,0 assumed to be in the top-left corner of input
	event->loc.y = WALL_SCREEN_HEIGHT - event->loc.y;
	event->delta.y = WALL_SCREEN_HEIGHT - event->delta.y;

	if (_wallView->IsCordsWithin(event->loc.x, event->loc.y) == false)
		return;

	// Transform the global display wall coordinates from shout into global
	// coordinates within the application, which uses the same coordinate system,
	// however, might be (significantly) smaller in physical screen size (.e.g 2x2 tiles)
	_wallView->GlobalToGridCoords((float *)&event->loc.x, (float *)&event->loc.y);
	_wallView->GlobalToGridCoords((float *)&event->delta.x, (float *)&event->delta.y);

	// Find all entities that the event "hits"
	vector<EntityHit> entityHits = _scenes[0]->TestForEntityHits(event->loc.x, event->loc.y);

	LOG(INFO) << "Num entity hits: " << entityHits.size();
	if (entityHits.size() == 0)
		// No entity hits within scene
		return;

	eventQueue->Push(make_tuple(entityHits[0].entity, *event));
}




