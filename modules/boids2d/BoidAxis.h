/*
 * Axis.h
 *
 *  Created on: May 11, 2011
 *      Author: arild
 */

#ifndef AXIS_H_
#define AXIS_H_

#include "FTGL/ftgl.h"
#include "Entity.h"
#include "Font.h"

class BoidAxis : public EntityEvent {
public:
	BoidAxis();
	virtual ~BoidAxis();

	void Set(int start, int stop, int tickSize);
	void Get(int *start, int *stop, int *tickSize);

	virtual void OnInit();
	virtual void OnLoop();
	virtual void OnRender();
	virtual void OnCleanup();
	virtual void Tap(float x, float y);
	virtual void ScrollDown(float speed);
	virtual void ScrollUp(float speed);
	virtual void SwipeLeft(float speed);
	virtual void SwipeRight(float speed);

private:
	int _start, _stop, _tickSize;
	Font *_font;
	void _DrawLinearTicks();
	void _DrawLogarithmicTicks();
};

#endif /* AXIS_H_ */
