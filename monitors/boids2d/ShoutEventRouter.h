/*
 * ShoutEventRouter.h
 *
 *  Created on: May 13, 2011
 *      Author: arild
 */

#ifndef SHOUTEVENTROUTER_H_
#define SHOUTEVENTROUTER_H_

#include <vector>
#include "boost/tuple/tuple.hpp"
#include "Queue.h"

using namespace std;
using namespace boost::tuples;

class EventSpace {
public:
	float x, y, w, h;
	EventSpace(float x_, float y_, float w_, float h_);
	bool IsWithin(float x, float y);
};

class TouchEvent {
public:
	float x, y;
	TouchEvent(float x_, float y_) : x(x_), y(y_) {};
};

typedef tuple<Queue<TouchEvent> *, EventSpace> RouterTupleType;

class ShoutEventRouter {
public:
	ShoutEventRouter();
	virtual ~ShoutEventRouter();
	Queue<TouchEvent> *Register(EventSpace eventSpace);

private:
	vector<RouterTupleType> _register;
};

#endif /* SHOUTEVENTROUTER_H_ */
