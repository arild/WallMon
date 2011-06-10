/*
 * SdlMouseEventFetcher.h
 *
 *  Created on: May 17, 2011
 *      Author: arild
 */

#ifndef SDLMOUSEEVENTFETCHER_H_
#define SDLMOUSEEVENTFETCHER_H_

#include "boost/tuple/tuple.hpp"
#include "EventSystemBase.h"
#include "Queue.h"

using namespace std;

class SdlMouseEventFetcher : public EventSystemBase {
public:
	SdlMouseEventFetcher();
	virtual ~SdlMouseEventFetcher();
	virtual void PollEvents();
	virtual bool InitEventSystem();
	virtual void WaitAndHandleNextEvent();

private:
	Queue<TT_touch_state_t *> *_queue;
	double _timestamp;
};

#endif /* SDLMOUSEEVENTFETCHER_H_ */
