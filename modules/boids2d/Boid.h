#ifndef BOID_H_
#define BOID_H_

#include <list>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <boost/tuple/tuple.hpp>
#include "Entity.h"
#include "BoidSharedContext.h"
#include "Font.h"

using namespace boost::tuples;
typedef tuple<float, float> TailTupleType;


class Boid: public virtual Entity, EntityShape {
public:
	BoidSharedContext *ctx;
	Boid(BoidSharedContext *ctx_);
	virtual ~Boid();
	virtual void OnInit();
	virtual void OnLoop();
	virtual void OnRender();
	virtual void OnCleanup();
	void HandleHit(TouchEvent &event);
private:
	float speedx, speedy;
	float _oldDestx, _oldDesty;
	int _maxTailLength;
	list<TailTupleType> _tail;
	Font _font;
	bool _isHighlighted;

	bool _visible;
	bool _IsDestinationReached(float destx, float desty);
	void _DrawBoid();
	void _DrawTail();
	void _DrawHighlight();
};

#endif /* BOID_H_ */
