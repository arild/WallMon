/*
 * TouchEvent.h
 *
 *  Created on: May 24, 2011
 *      Author: arild
 */

#ifndef TOUCHEVENT_H_
#define TOUCHEVENT_H_

#include <shout/shout.h>

class TouchEvent {
public:
	float x, y, realX, realY;
	shout_event_t *shoutEvent;
	bool isUp, visualizeOnly;
	virtual ~TouchEvent() {}
};

#endif /* TOUCHEVENT_H_ */
