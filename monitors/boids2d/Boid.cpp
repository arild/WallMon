#include <iostream>
#include <boost/thread/mutex.hpp>
#include <glog/logging.h>

#include "Boid.h"
#include "Fps.h"
using namespace std;

float maxX = 0, maxY = 0;

Boid::Boid()
{
	_oldDestx = -1;
	_oldDesty = -1;
	width = 2;
	height = 2;
	_quadric = gluNewQuadric();
}

Boid::~Boid()
{
	delete ctx;
}

void Boid::OnLoop()
{
	float destx, desty;

	// Get boid destination via thread-safe API
	ctx->GetDestination(&destx, &desty);

	if (destx > 100)
		destx = 100;
	if (desty > 100)
		desty = 100;

	if (destx > maxX) {
		LOG(INFO) << "X=" << destx;
		maxX = destx;
	}
	if (desty > maxY) {
		LOG(INFO) << "Y=" << desty;
		maxY = desty;
	}

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
			speedx = dx / FPS;
			speedy = dy / FPS;
		}
		// Physically move the boid
		tx += speedx;
		ty += speedy;
	}
}

//void Circle(float x, float y, float r, const Color& vColor)
//{
//	glBegin(GL_TRIANGLE_FAN);
//	glVertex2f(x, y);
//	for( float t, int n = 0; n <= 90; ++n, t = (float)n/90.f ) // increment by a fraction of the maximum
//	{
//		glVertex2f( x + sin( t * 2 * M_PI ) * r, y + cos( t * 2 * M_PI ) * r );
//	}
//	glEnd();
//}

void Boid::OnRender(SDL_Surface *screen)
{
	// all gl stuff
	glBegin(GL_QUADS);
	glColor3ub(ctx->red, ctx->green, ctx->blue);

//	glTranslatef(tx, ty, 0);
//	gluQuadricDrawStyle(_quadric, GLU_FILL );
//	gluSphere(_quadric, 2, 100, 100);

	glVertex2f(tx, ty);
	glVertex2f(tx, ty - height);
	glVertex2f(tx + width, ty - height);
	glVertex2f(tx + width, ty);

	glEnd();
}

void Boid::OnCleanup()
{

}

bool Boid::_IsDestinationReached(float destx, float desty)
{
	float dx = abs(tx - (float) destx);
	float dy = abs(ty - (float) desty);
	if (dx < 1. && dy < 1.)
		return true;
	return false;
}

