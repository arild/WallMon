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
	void SetView(double x, double y, double width, double height);
private:
	boost::thread _thread;
	bool _running;
	SDL_Surface *_screen;
	bool _updateOrtho;
	double _orthoLeft, _orthoRight, _orthoBottom, _orthoTop;

	void _InitSdlAndOpenGl();
	void _RenderForever();
	void _DrawAxis();
	void _DrawBoidDescription();
};

#endif /* BOIDS_H_ */
