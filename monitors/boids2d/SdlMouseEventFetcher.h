/*
 * SdlMouseEventFetcher.h
 *
 *  Created on: May 17, 2011
 *      Author: arild
 */

#ifndef SDLMOUSEEVENTFETCHER_H_
#define SDLMOUSEEVENTFETCHER_H_

#include "boost/tuple/tuple.hpp"
#include "EventHandlerBase.h"
#include "Queue.h"

using namespace std;
using namespace boost::tuples;

typedef tuple<float, float> TupleType;

class SdlMouseEventFetcher : public EventHandlerBase {
public:
	SdlMouseEventFetcher();
	virtual ~SdlMouseEventFetcher();
	virtual void PollEvents();
	virtual void _InitEventSystem();
	virtual void _WaitNextEvent(float *x, float *y);

	Queue<TupleType> *_queue;

};

#endif /* SDLMOUSEEVENTFETCHER_H_ */
