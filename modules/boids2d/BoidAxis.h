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
#include "ControlPanel.h"

class BoidAxis : public EntityEvent {
public:
	static ControlPanel *controlPanel;
	BoidAxis();
	virtual ~BoidAxis();

	void Set(int start, int stop, int tickSize);
	void Get(int *start, int *stop, int *tickSize);

	virtual void OnInit();
	virtual void OnLoop();
	virtual void OnRender();
	virtual void OnCleanup();
	void Tap(float x, float y);
	void ScrollDown(float speed);
	void ScrollUp(float speed);
	void SwipeLeft(float speed);
	void SwipeRight(float speed);

private:
	int _start, _stop, _tickSize;
	Font *_font;
	void _DrawLinearTicks();
	void _DrawLogarithmicTicks();
};

#endif /* AXIS_H_ */
