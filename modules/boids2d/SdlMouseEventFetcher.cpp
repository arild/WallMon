/*
 * SdlMouseEventFetcher.cpp
 *
 *  Created on: May 17, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include "SdlMouseEventFetcher.h"
#include "SDL/SDL.h"
#include "stdio.h"
#include "WallView.h"
#include "System.h"

SdlMouseEventFetcher::SdlMouseEventFetcher()
{
	_queue = new Queue<TT_touch_state_t *> (100);
	_timestamp = System::GetTimeInMsec();
	_isEventStreamActive = false;
}

SdlMouseEventFetcher::~SdlMouseEventFetcher()
{
	delete _queue;
}

bool SdlMouseEventFetcher::InitEventSystem()
{
	return true;
}

void SdlMouseEventFetcher::PollEvents()
{
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	TT_touch_state_t *obj = new TT_touch_state_t();
	obj->loc.x = mx * (float) (WALL_SCREEN_WIDTH / (float) 1600);
	obj->loc.y = my * (float) (WALL_SCREEN_HEIGHT / (float) 768);

	SDL_Event event;
	SDL_PumpEvents();
	while (SDL_PollEvent(&event)) {
		switch (event.type)
		{
		case SDL_KEYDOWN:
			_isEventStreamActive = true;
			obj->isUp = false;
			break;
		case SDL_KEYUP:
			_isEventStreamActive = false;
			obj->isUp = true;
			_queue->Push(obj);
			return;
			break;
		default:
			break;
		}
	}

	if (_isEventStreamActive == false)
		return;

	double ts = System::GetTimeInMsec();
	if (ts - _timestamp < 50)
		return;
	_timestamp = ts;
	_queue->Push(obj);
}

void SdlMouseEventFetcher::WaitAndHandleNextEvent()
{
	TT_touch_state_t *event = _queue->Pop();
	FilterAndRouteEvent(event);
}
