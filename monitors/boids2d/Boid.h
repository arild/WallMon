#ifndef BOID_H_
#define BOID_H_

#include <list>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <boost/tuple/tuple.hpp>
#include "IEntity.h"
#include "BoidSharedContext.h"

using namespace boost::tuples;
typedef tuple<float, float> TailTupleType;


class Boid: IEntity {
public:
	BoidSharedContext *ctx;
	Boid();
	virtual ~Boid();
	virtual void OnLoop();
	virtual void OnRender();
	virtual void OnCleanup();
	void HandleHit(TouchEvent &event);
private:
	float speedx, speedy;
	float _oldDestx, _oldDesty;
	int _maxTailLength;
	list<TailTupleType> _tail;

	bool _visible;
	GLUquadricObj *_quadric;
	bool _IsDestinationReached(float destx, float desty);
	void _DrawBoid();
	void _DrawTail();
};

#endif /* BOID_H_ */
