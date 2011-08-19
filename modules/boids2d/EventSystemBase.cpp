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
	eventQueue = new Queue<TouchEvent> (10000);
}

EventSystemBase::~EventSystemBase()
{
	delete eventQueue;
}

void EventSystemBase::SetWallView(WallView *wallView)
{
	_wallView = wallView;
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

void EventSystemBase::FilterAndRouteEvent(TT_touch_state_t *obj, bool isDown)
{
	float x = (float)obj->loc.x;
	float y = (float)obj->loc.y;

	// Convert y coordinate: 0,0 assumed to be in the top-left corner of input
	y = WALL_SCREEN_HEIGHT - y;

	if (_wallView->IsCordsWithin(x, y) == false)
		return;

	// Transform the global display wall coordinates from shout into global
	// coordinates within the application, which uses the same coordinate system,
	// however, might be (significantly) smaller in physical screen size (.e.g 2x2 tiles)
	_wallView->GlobalToGridCoords(&x, &y);

	TouchEvent event;
	event.isDown = isDown;
	event.x = x;
	event.y = y;
	event.movedDistance = obj->movedDistance;
	event.radius = obj->radius;
	event.timestampSec = obj->time;
	event.timestampPreviousSec = (float)obj->lastUpdated;

//	Scene *scene = Scene::TestForSceneHit(event.realX, event.realY);
//	if (scene == NULL)
//		// Coordinates not within any scene
//		return;
	event.visualizeOnly = true;
	eventQueue->Push(event);
	event.visualizeOnly = false;

	// Transform global coordinates within the application to match coordinate
	// system used within the relevant scene, which could be anything
//	scene->RealToVirtualCoords(event.realX, event.realY, &event.x, &event.y);
//	item = make_tuple(scene, event);
//	eventQueue->Push(item);
}




