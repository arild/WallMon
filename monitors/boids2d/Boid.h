#ifndef BOID_H_
#define BOID_H_

#include "IEntity.h"
#include "BoidSharedContext.h"

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

class Boid: IEntity {
public:
	BoidSharedContext *ctx;
	Boid();
	virtual ~Boid();
	virtual void OnLoop();
	virtual void OnRender();
	virtual void OnCleanup();

private:
	float speedx, speedy;
	float _oldDestx, _oldDesty;
	bool _visible;
	GLUquadricObj *_quadric;
	bool _IsDestinationReached(float destx, float desty);
	void _DrawAxis();
};

#endif /* BOID_H_ */
