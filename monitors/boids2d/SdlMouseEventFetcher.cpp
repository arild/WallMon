/*
 * SdlMouseEventFetcher.cpp
 *
 *  Created on: May 17, 2011
 *      Author: arild
 */

#include "SdlMouseEventFetcher.h"
#include "SDL/SDL.h"
#include "stdio.h"
#include "WallView.h"
#include "System.h"

SdlMouseEventFetcher::SdlMouseEventFetcher()
{
	_queue = new Queue<TT_touch_state_t *>(100);
	_timestamp = System::GetTimeInMsec();
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
	double ts = System::GetTimeInMsec();
	if (ts - _timestamp < 50)
		return;
	_timestamp = ts;

	SDL_Event event;
	SDL_PumpEvents();
//	if (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_EVENTMASK (SDL_MOUSEBUTTONDOWN)) == 1) {
//		float x = (float)event.button.x;
//		float y = (float)event.button.y;
//
//		x *= (WALL_SCREEN_WIDTH / (float)1600);
//		y *= (WALL_SCREEN_HEIGHT / (float)768);
//
//		printf("Mouse event: x=%.2f | y=%.2f\n", x, y);
//		_queue->Push(make_tuple(x, y));
//	}

	int mx, my;
	SDL_GetMouseState(&mx, &my);
	TT_touch_state_t *obj = new TT_touch_state_t();
	obj->loc.x = mx * (float)(WALL_SCREEN_WIDTH / (float)1600);
	obj->loc.y = my * (float)(WALL_SCREEN_HEIGHT / (float)768);

//	printf("Sending mouse event: x=%.2f | y=%.2f\n", obj->loc.x, obj->loc.y);
	_queue->Push(obj);
}

void SdlMouseEventFetcher::WaitAndHandleNextEvent()
{
//	printf("WaitAndHandleNextEvent()\n");
	TT_touch_state_t *event = _queue->Pop();
//	printf("Mouse event received: x=%.2f | y=%.2f\n", event->loc.x, event->loc.y);
	FilterAndRouteEvent(event, true);
}
