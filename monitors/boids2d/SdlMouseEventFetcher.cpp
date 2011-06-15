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
	_queue = new Queue<tuple<TT_touch_state_t *, bool> > (100);
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
			break;
		case SDL_KEYUP:
			_isEventStreamActive = false;
			_queue->Push(make_tuple(obj, false));
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
	_queue->Push(make_tuple(obj, true));
}

void SdlMouseEventFetcher::WaitAndHandleNextEvent()
{
	tuple<TT_touch_state_t *, bool> t = _queue->Pop();
	FilterAndRouteEvent(t.get<0> (), t.get<1> ());
}
