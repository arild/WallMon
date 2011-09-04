/*
 * SdlMouseEventFetcher.cpp
 *
 *  Created on: May 17, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include "SDL/SDL.h"
#include "shout/shout.h"
#include "shout/event-types/touch-events.h"
#include "SdlMouseEventFetcher.h"
#include "System.h"
#include "stdio.h"
#include "WallView.h"

SdlMouseEventFetcher::SdlMouseEventFetcher()
{
	_timestamp = System::GetTimeInMsec();
	_isEventStreamActive = false;
	_eventId = 0;
}

SdlMouseEventFetcher::~SdlMouseEventFetcher()
{
}

bool SdlMouseEventFetcher::InitEventSystem()
{
	return true;
}

/**
 * Simulates shout events by using a mouse and keyboard, and SDL.
 */
void SdlMouseEventFetcher::PollEvents()
{
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	TouchEvent event;
	event.x = mx * (float) (WALL_SCREEN_WIDTH / (float) 1600);
	event.y = my * (float) (WALL_SCREEN_HEIGHT / (float) 768);

	SDL_Event sdlEvent;
	SDL_PumpEvents();
	while (SDL_PollEvent(&sdlEvent)) {
		switch (sdlEvent.type)
		{
		case SDL_KEYDOWN: // Occurs only once when a key is pressed and kept down
			_isEventStreamActive = true;
			break;
		case SDL_KEYUP:
			LOG(INFO) << "SDL KEY UP";
			_isEventStreamActive = false;
//			event.shoutEvent = CreateShoutEvent(_eventId);
//			_queue.Push(event);
			event.shoutEvent = create_touch_remove_event(_eventId, 0);
			_queue.Push(event);
			_eventId += 1;
			return;
		default:
			break;
		}
	}

	if (_isEventStreamActive == false)
		return;

	double ts = System::GetTimeInMsec();
	if (ts - _timestamp < 10)
		return;
	_timestamp = ts;
	event.shoutEvent = CreateShoutEvent(_eventId);
	_queue.Push(event);
}

void SdlMouseEventFetcher::WaitAndHandleNextEvent()
{
	TouchEvent event = _queue.Pop();
	FilterAndRouteEvent(event);
}

shout_event_t *SdlMouseEventFetcher::CreateShoutEvent(int eventId)
{
	return create_calibrated_touch_location_event_v2(eventId, kTouch_evt_first_detect_flag | kTouch_evt_last_detect_flag, 0, 0, 2, 0, 0);
}
