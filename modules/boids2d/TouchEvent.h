/*
 * TouchEvent.h
 *
 *  Created on: May 24, 2011
 *      Author: arild
 */

#ifndef TOUCHEVENT_H_
#define TOUCHEVENT_H_

class TouchEvent {
public:
	int touchId;
	bool isDown;
	bool visualizeOnly;
	float x, y;
	float radius, movedDistance;
	float timestampSec, timestampPreviousSec;
	virtual ~TouchEvent() {}
};

#endif /* TOUCHEVENT_H_ */
