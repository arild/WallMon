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

SdlMouseEventFetcher::SdlMouseEventFetcher()
{
	_queue = new Queue<TupleType>(100);
}

SdlMouseEventFetcher::~SdlMouseEventFetcher()
{
	delete _queue;
}

void SdlMouseEventFetcher::_InitEventSystem()
{
}

void SdlMouseEventFetcher::PollEvents()
{
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
	float x = mx * (float)(WALL_SCREEN_WIDTH / (float)1600);
	float y = my * (float)(WALL_SCREEN_HEIGHT / (float)768);

	//printf("Mouse event: x=%.2f | y=%.2f\n", x, y);
	_queue->Push(make_tuple(x, y));
}

void SdlMouseEventFetcher::_WaitNextEvent(float *x, float *y)
{
	printf("Waiting for mouse event\n");
	TupleType t = _queue->Pop();
	*x = t.get<0>();
	*y = t.get<1>();
	printf("Mouse event received: x=%.2f | y=%.2f\n", *x, *y);
}
