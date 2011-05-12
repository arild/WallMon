/*
 * Axis.h
 *
 *  Created on: May 11, 2011
 *      Author: arild
 */

#ifndef AXIS_H_
#define AXIS_H_

#include "Entity.h"

class BoidAxis : Entity {
public:
	BoidAxis();
	virtual ~BoidAxis();

	void Set(int start, int stop, int tickSize);
	void Get(int *start, int *stop, int *tickSize);

	virtual void OnLoop();
	virtual void OnRender();
	virtual void OnCleanup();

private:
	int _start, _stop, _tickSize;

};

#endif /* AXIS_H_ */
