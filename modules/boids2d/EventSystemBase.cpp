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

void EventSystemBase::FilterAndRouteEvent(TouchEvent &event)
{
	// Convert y coordinate: 0,0 assumed to be in the top-left corner of input
	event.y = WALL_SCREEN_HEIGHT - event.y;

	if (_wallView->IsCordsWithin(event.x, event.y) == false)
		return;

	// Transform the global display wall coordinates from shout into global
	// coordinates within the application, which uses the same coordinate system,
	// however, might be (significantly) smaller in physical screen size (.e.g 2x2 tiles)
	_wallView->GlobalToGridCoords((float *)&event.x, (float *)&event.y);

	// Find all entities that the event "hits"
	vector<EntityHit> entityHits = Scene::GetAllEntityHits(event.x, event.y);

	event.realX = event.x;
	event.realY = event.y;

	if (entityHits.size() == 0) {
		// No entity hits within scene
		event.visualizeOnly = true;
		eventQueue->Push(make_tuple((Entity *)NULL, event));
		return;
	}

	event.visualizeOnly = false;
	event.x = entityHits[0].virtX;
	event.y = entityHits[0].virtY;

	eventQueue->Push(make_tuple(entityHits[0].entity, event));
}




