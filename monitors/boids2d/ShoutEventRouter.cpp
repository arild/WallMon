/*
 * ShoutEventRouter.cpp
 *
 *  Created on: May 13, 2011
 *      Author: arild
 */

#include "ShoutEventRouter.h"

EventSpace::EventSpace(float x_, float y_, float w_, float h_) : x(x_), y(y_), w(w_), h(h_)
{

}

bool EventSpace::IsWithin(float x, float y)
{
	return true;
}

ShoutEventRouter::ShoutEventRouter()
{

}

ShoutEventRouter::~ShoutEventRouter()
{

}

Queue<TouchEvent> *ShoutEventRouter::Register(EventSpace eventSpace)
{
	Queue<TouchEvent> *q = new Queue<TouchEvent>(100);
	RouterTupleType t = make_tuple(q, eventSpace);
	_register.push_back(t);
	return q;
}






