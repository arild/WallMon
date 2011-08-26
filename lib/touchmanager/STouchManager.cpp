//  STouchManager.cxx
//  Another attempt at a useful C/C++ touch-manager.
//
//  Created by Daniel Stødle on 26/10/2010.
//  Copyright 2010 Yellow Lemon Software. All rights reserved.
//

#include "STouchManager.h"
#include <stdlib.h>
#include <string.h>

static STouchManager *mgr = 0;

STouchManager* STouchManager::sharedTouchManager(void)
{
	if (!mgr)
		mgr = new STouchManager(0, 0);
	return mgr;
}

STouchManager::STouchManager(void *refcon, touchManagerCallback_t callback, uint32_t wantedSenderID) :
	refcon(refcon), touchesDown(), touchesUp(), wantedSenderID(wantedSenderID), callback(callback)
{
	pthread_mutex_init(&mgrLock, 0);
}

STouchManager::~STouchManager(void)
{
	pthread_mutex_destroy(&mgrLock);
}

void STouchManager::handleEvent(shout_event_t *evt, float virtX, float virtY)
{
	bool shouldRunCallback = false;
	lock();
	if (evt->type == kEvt_type_calibrated_touch_location) {
		bool isLast = false;
		TT_touch_state_t *state = newTouchStateForEvent(evt, isLast, virtX, virtY), *existingState = 0;
		if (state) {
			oidTouchMap_t::iterator it;
			it = oidTouchMap.find(state->oid);
			if (it != oidTouchMap.end()) {
				existingState = it->second;
				updateTouch(existingState, state);
				free(state);
			} else {
				oidTouchMap.insert(std::make_pair(state->oid, state));
				touchesDown.push_back(state);
			}
			if (isLast)
				shouldRunCallback = true;
		}
	} else if (evt->type == kEvt_type_touch_remove) {
		uint32_t oid, senderID;
		if (parse_touch_remove_event(evt, &oid, &senderID) == 0) {
			if (senderID == (uint32_t) wantedSenderID || wantedSenderID == -1) {
				oidTouchMap_t::iterator map_it;
				TT_touch_state_t *state = 0;
				map_it = oidTouchMap.find(oid);
				if (map_it != oidTouchMap.end()) {
					state = map_it->second;
					oidTouchMap.erase(map_it);
					for (touchVector_t::iterator vec_it = touchesDown.begin(); vec_it
							!= touchesDown.end(); ++vec_it) {
						if (*vec_it == state) {
							touchesDown.erase(vec_it);
							break;
						}
					}
					state->remove = true;
					touchesUp.push_back(state);
					shouldRunCallback = true;
				}
			}
		}
	}
	unlock();
	if (shouldRunCallback)
		runCallback();
}

void STouchManager::runCallback(void)
{
	lock();
	if (callback)
		callback(refcon, touchesDown, touchesUp);
	for (size_t i = 0; i < touchesUp.size(); i++)
		free(touchesUp[i]);
	touchesUp.clear();
	unlock();
}

void STouchManager::updateTouch(TT_touch_state_t *obj, TT_touch_state_t *newObj)
{
	obj->delta = newObj->loc - obj->loc;
	obj->loc = newObj->loc;
	obj->radius = newObj->radius;
	obj->oid = newObj->oid;
	obj->movedDistance += obj->delta.length();
	obj->time += shout_double_time() - obj->lastUpdated;
	obj->wasUpdated = true;
	obj->lastUpdated = shout_double_time();
}

TT_touch_state_t* STouchManager::newTouchStateForEvent(shout_event_t *evt, bool &isLast, float virtX, float virtY)
{
	TT_touch_state_t *ts = (TT_touch_state_t*) calloc(sizeof(TT_touch_state_t), 1);
	uint32_t locFlags, senderID;
	float x, y, radius;

	if (parse_touch_location_event_v2(evt, &locFlags, &x, &y, &radius, 0, &senderID) == 0) {
		if (senderID == (uint32_t) wantedSenderID || wantedSenderID == -1) {
			ts->loc = WVPoint2d(virtX, virtY);
			ts->radius = radius;
			ts->oid = evt->refcon;
			ts->lastUpdated = shout_double_time();
			ts->wasUpdated = true;
			isLast = locFlags & kTouch_evt_last_detect_flag;
			return ts;
		}
	}
	free(ts);
	return 0;
}

double STouchManager::maxDistanceBetweenTouches(size_t &a, size_t &b)
{
	double maxDistance = 0, distance;
	for (size_t i = 0; i < touchesDown.size(); i++) {
		for (size_t j = i + 1; j < touchesDown.size(); j++) {
			distance = touchesDown[i]->loc.distance(touchesDown[j]->loc);
			if (distance > maxDistance) {
				a = i;
				b = j;
				maxDistance = distance;
			}
		}
	}
	return maxDistance;
}

void STouchManager::lock(void)
{
	pthread_mutex_lock(&mgrLock);
}
void STouchManager::unlock(void)
{
	pthread_mutex_unlock(&mgrLock);
}
