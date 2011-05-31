/**
 * @file   BoidSharedContext.cpp
 * @Author Arild Nilsen
 * @date   April, 2011
 *
 * Simple encapsulation for sharing the coordinates of a boid
 */

#include "BoidSharedContext.h"

int BoidSharedContext::tailLength = 0;
bool BoidSharedContext::showCpuBoid = true;
bool BoidSharedContext::showMemoryBoid = true;
bool BoidSharedContext::showNetworkBoid = true;
bool BoidSharedContext::showStorageBoid = true;

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

void BoidSharedContext::SetTailLength(int length)
{
	scoped_lock lock(_mutex);
	_tailLength = length;
}

int BoidSharedContext::GetTailLength()
{
	scoped_lock lock(_mutex);
	return _tailLength;
}




