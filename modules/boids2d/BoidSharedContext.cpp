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
bool BoidSharedContext::useLogarithmicAxis = false;
bool BoidSharedContext::showCpuBoid = true;
bool BoidSharedContext::showMemoryBoid = true;
bool BoidSharedContext::showIoInBoid = true;
bool BoidSharedContext::showIoOutBoid = true;

BoidSharedContext::BoidSharedContext(int red_, int green_, int blue_, Shape shape_)
{
	red = red_;
	green = green_;
	blue = blue_;
	boidShape = shape_;

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

void BoidSharedContext::EnableHighlight(int highlightNumber)
{
	_isHighlighted = true;
	_highlightNumber = highlightNumber;
}

int BoidSharedContext::GetHighlightNumber()
{
	return _highlightNumber;
}

void BoidSharedContext::DisableHighlight()
{
	_isHighlighted = false;
}

bool BoidSharedContext::IsHighlighted()
{
	return _isHighlighted;
}






