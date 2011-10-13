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
	_font.SetFontType(FONT_MONO);
	_font.SetFontSize(2);
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

	// Align (if necessary) the boids with the axes. This can reduce the exactness
	// of the visualization. For example, given a utilization of
	destx = fmax(destx, width/(float)2);
	destx = fmin(destx, 100 - width/(float)2);
	desty = fmax(desty, height/(float)2);
	desty = fmin(desty, 100 - height/(float)2);

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

	if (_visible) {
		// Normally this would be located in OnRender(), however, in order to
		// draw all boids first, then label them with a number, this works as
		// a simple solution.
		_isHighlighted = ctx->IsHighlighted();
		if (_isHighlighted) {
			width += 1;
			height += 1;
		}
		glColor3ub(ctx->red, ctx->green, ctx->blue);
		_DrawBoid();
		_DrawTail();
	}

	int tailLength = (int)BoidSharedContext::tailLength;
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
	if (_visible == false)
		return;

	if (_isHighlighted) {
		_DrawHighlight();
		width -= 1;
		height -= 1;
	}
}

void Boid::OnCleanup()
{

}

bool Boid::_IsDestinationReached(float destx, float desty)
{
	float dx = fabs(tx - (float) destx);
	float dy = fabs(ty - (float) desty);
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
	case DIAMOND:
		if (BoidSharedContext::showMemoryBoid == false)
			return;
		break;
	case TRIANGLE_DOWN:
		if (BoidSharedContext::showIoInBoid == false)
			return;
		break;
	case TRIANGLE_UP:
		if (BoidSharedContext::showIoOutBoid == false)
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

void Boid::_DrawHighlight()
{
	stringstream ss;
	ss << ctx->GetHighlightNumber();
	glColor3f(1, 0, 0);
	// tx and ty refers to origo of boids
	_font.RenderText(ss.str(), tx, ty - height/(float)2 + 0.2, true, false);
//	switch (ctx->boidShape)
//	{
//	case QUAD:
//		if (BoidSharedContext::showCpuBoid == false)
//			return;
//		break;
//	case TRIANGLE:
//		if (BoidSharedContext::showMemoryBoid == false)
//			return;
//		break;
//	case DIAMOND:
//		if (BoidSharedContext::showNetworkBoid == false)
//			return;
//		break;
//	case POLYGON:
//		if (BoidSharedContext::showStorageBoid == false)
//			return;
//		break;
//	}
}











