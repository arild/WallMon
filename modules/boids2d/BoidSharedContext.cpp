/**
 * @file   BoidSharedContext.cpp
 * @Author Arild Nilsen
 * @date   April, 2011
 *
 * Encapsulation for sharing the coordinates of a boid and other
 * configurations related to boids
 */

#include "BoidSharedContext.h"

int BoidSharedContext::tailLength = 0;
bool BoidSharedContext::showCpuBoid = false;
bool BoidSharedContext::showMemoryBoid = false;
bool BoidSharedContext::showNetworkBoid = false;
bool BoidSharedContext::showStorageBoid = false;
BoidView BoidSharedContext::boidViewToShow = BOID_TYPE_PROCESS_NAME;

BoidSharedContext::BoidSharedContext(int red_, int green_, int blue_, Shape shape_, BoidView boidType_)
{
	red = red_;
	green = green_;
	blue = blue_;
	boidShape = shape_;
	boidView = boidType_;
	_showDisplayText = false;
	boidInfo.procName = "proc name";
	boidInfo.hostName = "host name";
}

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

void BoidSharedContext::ShowDisplayText()
{
	_showDisplayText = true;
}

void BoidSharedContext::DisableDisplayText()
{
	_showDisplayText = false;
}






