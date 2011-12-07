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
#include "shout/event-types/shout-std-events.h"
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
	mx *= (float) (WALL_SCREEN_WIDTH / (float) 1600);
	my *= (float) (WALL_SCREEN_HEIGHT / (float) 768);

	SDL_Event sdlEvent;
	SDL_PumpEvents();
	while (SDL_PollEvent(&sdlEvent)) {
		switch (sdlEvent.type)
		{
		case SDL_MOUSEBUTTONDOWN: // Occurs only once when a key is pressed and kept down
			_isEventStreamActive = true;
			break;
		case SDL_MOUSEBUTTONUP:
			_isEventStreamActive = false;
			_queue.Push(create_touch_remove_event(_eventId, 0));
			_eventId += 1;
			return;
		case SDL_KEYDOWN:
			uint8_t loc_flags;
			uint32_t sound_type;
			_queue.Push(shout_create_sound_location_event(sound_type, loc_flags, 0, 0, 0, 0));
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
	_queue.Push(CreateShoutEvent(_eventId, (float)mx, (float)my));
}

void SdlMouseEventFetcher::WaitAndHandleNextEvent()
{
	shout_event_t *event = _queue.Pop();
	FilterAndRouteEvent(event);
}

shout_event_t *SdlMouseEventFetcher::CreateShoutEvent(int eventId, float x, float y)
{
	return create_calibrated_touch_location_event_v2(eventId, kTouch_evt_first_detect_flag | kTouch_evt_last_detect_flag, x, y, 2, 0, 0);
}
