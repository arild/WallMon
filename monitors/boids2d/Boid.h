#ifndef BOID_H_
#define BOID_H_

#include "Entity.h"
#include "BoidSharedContext.h"

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

class Boid: Entity {
public:
	BoidSharedContext *ctx;
	Boid();
	virtual ~Boid();
	virtual void OnLoop();
	virtual void OnRender(SDL_Surface *screen);
	virtual void OnCleanup();

private:
	float _oldDestx, _oldDesty;
	bool _visible;
	bool _IsDestinationReached(float destx, float desty);
	GLUquadricObj *_quadric;
};

#endif /* BOID_H_ */
