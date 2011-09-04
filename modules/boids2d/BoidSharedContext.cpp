/**
 * @file   BoidSharedContext.cpp
 * @Author Arild Nilsen
 * @date   April, 2011
 *
 * Encapsulation for sharing the coordinates of a boid and other
 * configurations related to boids
 */

#include "BoidSharedContext.h"

float BoidSharedContext::tailLength = 0.;
bool BoidSharedContext::showCpuBoid = true;
bool BoidSharedContext::showMemoryBoid = true;
bool BoidSharedContext::showNetworkBoid = true;
bool BoidSharedContext::showStorageBoid = true;
BoidView BoidSharedContext::boidViewToShow = BOID_TYPE_PROCESS_NAME;

BoidSharedContext::BoidSharedContext(int red_, int green_, int blue_, Shape shape_, BoidView boidType_)
{
	red = red_;
	green = green_;
	blue = blue_;
	boidShape = shape_;
	boidView = boidType_;
	_isHighlighted = false;
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

void BoidSharedContext::SetTailLength(float length)
{
	scoped_lock lock(_mutex);
	_tailLength = length;
}


float BoidSharedContext::GetTailLength()
{
	scoped_lock lock(_mutex);
	return _tailLength;
}

void BoidSharedContext::EnableHighlight()
{
	_isHighlighted = true;
}

void BoidSharedContext::DisableHighlight()
{
	_isHighlighted = false;
}

bool BoidSharedContext::IsHighlighted()
{
	return _isHighlighted;
}






