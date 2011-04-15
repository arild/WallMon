#ifndef BOIDS_H_
#define BOIDS_H_

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <boost/thread.hpp>
#include "Boid.h"

using namespace std;

class BoidsApp {
public:
	BoidsApp();
	virtual ~BoidsApp();
	void Start();
	void Stop();
	void CreateBoid(double startX, double startY, BoidSharedContext *ctx);
	void RemoveBoid(Boid *boid);
private:
	boost::thread _thread;
	bool _running;
	SDL_Surface* _screen;
	void _InitSdlAndOpenGl();
	void _RenderForever();
	void _DrawAxis();
};

#endif /* BOIDS_H_ */
