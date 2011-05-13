/**
 * @file   BoidSharedContext.cpp
 * @Author Arild Nilsen
 * @date   April, 2011
 *
 * Simple encapsulation for sharing the coordinates of a boid
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
