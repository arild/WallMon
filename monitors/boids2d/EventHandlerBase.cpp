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

EventHandlerBase::EventHandlerBase()
{
	_outputQueue = new Queue<TouchEvent> (10000);
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

Queue<TouchEvent> *EventHandlerBase::GetOutputQueue()
{
	return _outputQueue;
}

void EventHandlerBase::Register(Scene *scene)
{
	_scenes.push_back(scene);
}

void EventHandlerBase::_HandleEventsForever()
{
	float x, y;
	_InitEventSystem();
	while (_running) {
		_WaitNextEvent(&x, &y);
		_FilterAndRouteEvent(x, y);
	}
}

void EventHandlerBase::_FilterAndRouteEvent(float x, float y)
{
	// Convert y coordinate: 0,0 assumed to be in the top-left corner of input
	y = WALL_SCREEN_HEIGHT - y;

	WallView w(3, 1, 2, 2);
	if (w.IsCordsWithin(x, y) == false)
		return;

	LOG(INFO) << "Event: x=" << x << " | y=" << y;
	TouchEvent event;
	w.GlobalToGridCoords(&x, &y);
	event.realX = x;
	event.realY = y;
	event.sceneX = x;
	event.sceneY = y;

	LOG(INFO) << "Maps to: x=" << x << " | y=" << y;

	event.scene = _GlobalCoordsToScene(event.realX, event.realY);
	if (event.scene == NULL) {
		// Coordinates not within any scene
		//LOG_EVERY_N(INFO, 100) << "event discarded";
		LOG(INFO) << "event discarded";
		return;
	}

	_outputQueue->Push(event);
}

Scene *EventHandlerBase::_GlobalCoordsToScene(float x, float y)
{
	BOOST_FOREACH(Scene *s, Scene::scenes)
	{
		if (x >= s->x && x <= s->x + s->w && y >= s->y && y <= s->y + s->h)
			return s;
	}
	return NULL;
}



