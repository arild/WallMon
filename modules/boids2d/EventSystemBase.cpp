/*
 * EventHandlerBase.cpp
 *
 *  Created on: May 17, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include <boost/foreach.hpp>
#include <shout/shout.h>
#include "shout/event-types/touch-events.h"
#include "Config.h"
#include "EventSystemBase.h"

typedef void (EventSystemBase::*HandleTouchesPtr)(touchVector_t &down, touchVector_t &up);

EventSystemBase::EventSystemBase()
{
	HandleTouchesPtr handle = &EventSystemBase::_HandleTouchesCallback;
	touchManagerCallback_t callback = *(touchManagerCallback_t*) &handle;
	_touchManager = new STouchManager(this, callback);
	eventQueue = new Queue<EventQueueItem> (10000);
	_currentEntity = NULL;
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

void EventSystemBase::FilterAndRouteEvent(shout_event_t *event)
{
	uint32_t locFlags, senderID, eventId;
	float x, y, radius;
	bool isLast;
	if (event->type == kEvt_type_calibrated_touch_location) {
		isLast = false;
		if (parse_touch_location_event_v2(event, &locFlags, &x, &y, &radius, 0, &senderID) != 0) {
			// Failed parsing shout event
			LOG(WARNING) << "failed parsing touch event";
			return;
		}
		eventId = event->refcon;
	}
	else if (event->type == kEvt_type_touch_remove) {
		isLast = true;
		if (parse_touch_remove_event(event, &eventId, &senderID) != 0) {
			LOG(WARNING) << "failed parsing touch remove event";
			return;
		}
	}
	else {
		LOG(WARNING) << "Unknown touch event type";
		return;
	}

	// Convert y coordinate: 0,0 assumed to be in the top-left corner of input
	y = WALL_SCREEN_HEIGHT - y;

	if (isLast == false && _wallView->IsCordsWithin(x, y) == false)
		// Event not within the defined grid (on the display wall)
		return;

	// Transform the global display wall coordinates from shout into global
	// coordinates within the application, which uses the same coordinate system,
	// however, might be (significantly) smaller in physical screen size (.e.g. 2x2 tiles)
	_wallView->GlobalToGridCoords((float *) &x, (float *) &y);

	// Find all entities that the event "hits"
	vector<EntityHit> entityHits = Scene::GetAllEntityHits(x, y);
	int numEntityHits = entityHits.size();

	// No entity hits within scene
	if (!isLast) {
		TT_touch_state_t e;
		e.loc.x = x;
		e.loc.y = y;
		e.visualizeOnly = true;
		eventQueue->Push(make_tuple((Entity *) NULL, e));
	}

	// Associate the id of the event with an entity
	EventIdMap::iterator it = _eventIdMap.find(eventId);
	Scene *scene = NULL;
	if (it != _eventIdMap.end()) {
		// Event id already associated with entity. Forward to
		// touch manager regardless whether the entity is hit or not
		_currentEntity = it->second.get<0>();
		scene = it->second.get<1>();
		if (event->type == kEvt_type_touch_remove) {
			// Special case: the event is a remove event (the last one)
			_eventIdMap.erase(eventId);
		}
	}
	else if (numEntityHits == 0) {
		// Id not previously associated and no entities' hit, discard event
		return;
	}
	else if (numEntityHits > 0) {
		// Id not previously associated, but entity hits available.
		_currentEntity = entityHits[0].entity;
		scene = entityHits[0].scene;
		_eventIdMap[eventId] = make_tuple(_currentEntity, scene);
	}

	// Transform grid coordinates to virtual entity coordinates
	scene->RealToVirtualCoords(x, y, &x, &y);

	// Re-create shout event for compatibility with touch-manager
	if (event->type == kEvt_type_calibrated_touch_location)
		event = create_calibrated_touch_location_event_v2(eventId,
				kTouch_evt_first_detect_flag | kTouch_evt_last_detect_flag, x, y, radius, 0, senderID);
	else if (event->type == kEvt_type_touch_remove)
		event = create_touch_remove_event(eventId, 0);
	else
		return;
	_touchManager->handleEvent(event);
}

void EventSystemBase::_HandleTouchesCallback(touchVector_t & down, touchVector_t & up)
{
	for (int i = 0; i < down.size(); i++) {
		TT_touch_state_t *obj = down[i];
		if (obj->wasUpdated) {
			obj->remove = false;
			_HandleTouch(*obj);
		}
	}
	for (int i = 0; i < up.size(); i++) {
		TT_touch_state_t *obj = up[i];
		if (obj->wasUpdated) {
			obj->remove = true;
			_HandleTouch(*obj);
		}
	}
}

void EventSystemBase::_HandleTouch(TT_touch_state_t & event)
{
	eventQueue->Push(make_tuple(_currentEntity, event));
}

