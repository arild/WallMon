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
	int id;
	bool isUp;
	float x, y, realX, realY;
	float radius;
	float timestampSec, timestampPreviousSec;
	virtual ~TouchEvent() {}
};

#endif /* TOUCHEVENT_H_ */
