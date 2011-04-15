/*
 * BoidSharedContext.cpp
 *
 *  Created on: Apr 10, 2011
 *      Author: arild
 */

#include "BoidSharedContext.h"

void BoidSharedContext::SetDestination(float x, float y)
{
	scoped_lock lock(_mutex);
	_destx = x;
	_desty = y;
}

void BoidSharedContext::GetDestination(float *x, float *y)
{
	scoped_lock lock(_mutex);
	*x = _destx;
	*y = _desty;
}
