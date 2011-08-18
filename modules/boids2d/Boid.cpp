/**
 * @Author Arild Nilsen
 * @date   April, 2011
 *
 * Implements the boids in the animation.
 */

#include <iostream>
#include <boost/thread/mutex.hpp>
#include <boost/foreach.hpp>
#include <glog/logging.h>

#include "Boid.h"
#include "Fps.h"

using namespace std;

Boid::Boid(BoidSharedContext *ctx_)
{
	ctx = ctx_;
	entityShape = ctx->boidShape;
	centerShape = true;
	_oldDestx = -1;
	_oldDesty = -1;
	width = 2;
	height = 2;
	tx = 0;
	ty = 0;
	_visible = false;
}

Boid::~Boid()
{
	delete ctx;
}

void Boid::OnInit()
{

}

void Boid::OnLoop()
{
	float destx, desty;

	// Get boid destination via thread-safe API
	ctx->GetDestination(&destx, &desty);
	if (destx < 0 || desty < 0) {
		_visible = false;
		return;
	}
	_visible = true;

	if (destx > 100)
		destx = 100;
	if (desty > 100)
		desty = 100;

	if (_IsDestinationReached(destx, desty)) {
		speedx = 0.;
		speedy = 0.;
	} else {
		if (destx != _oldDestx || desty != _oldDesty) {
			// One or more of the destination coordinates have changed -> update speed
			_oldDestx = destx;
			_oldDesty = desty;
			float dx = destx - tx;
			float dy = desty - ty;
			speedx = dx / MAX_FPS;
			speedy = dy / MAX_FPS;
		}
		// Physically move the boid
		tx += speedx;
		ty += speedy;
	}

	int tailLength = BoidSharedContext::tailLength;
	if (tailLength == 0) {
		_tail.clear();
		return;
	}
	if (_tail.size() == tailLength)
		_tail.pop_back();
	_tail.push_front(make_tuple(tx, ty));
}

void Boid::OnRender()
{
	// Filter for different metrics
	if (ctx->boidType != BoidSharedContext::boidTypeToShow)
		return;
	// Domain specific filter, .e.g utilization less than a threshold
	if (_visible == false)
		return;

	glColor3ub(ctx->red, ctx->green, ctx->blue);
	_DrawBoid();
	_DrawTail();
}

void Boid::OnCleanup()
{

}

bool Boid::_IsDestinationReached(float destx, float desty)
{
	float dx = abs(tx - (float) destx);
	float dy = abs(ty - (float) desty);
	if (dx < 0.5 && dy < 0.5)
		return true;
	return false;
}

void Boid::_DrawBoid()
{
	switch (ctx->boidShape)
	{
	case QUAD:
		if (BoidSharedContext::showCpuBoid == false)
			return;
		break;
	case TRIANGLE:
		if (BoidSharedContext::showMemoryBoid == false)
			return;
		break;
	case DIAMOND:
		if (BoidSharedContext::showNetworkBoid == false)
			return;
		break;
	case POLYGON:
		if (BoidSharedContext::showStorageBoid == false)
			return;
		break;
	}
	DrawEntityShape();
}

void Boid::HandleHit(TouchEvent & event)
{
	ctx->red = 255;
	ctx->green = 255;
	ctx->blue = 255;
}

void Boid::_DrawTail()
{
	glLineWidth(2);
	glBegin(GL_LINE_STRIP);
	BOOST_FOREACH(TailTupleType t, _tail)
	{
		float x = t.get<0>();
		float y = t.get<1>();
		glVertex3f(x, y, 0);
	}
	glEnd();
}













