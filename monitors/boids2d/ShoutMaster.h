/*
 * ShoutEventRouter.h
 *
 *  Created on: May 13, 2011
 *      Author: arild
 */

#ifndef SHOUTEVENTROUTER_H_
#define SHOUTEVENTROUTER_H_

#include "EventHandlerBase.h"
#include <shout/event-types/touch-events.h>

using namespace std;


class ShoutMaster : public EventHandlerBase {
public:
	ShoutMaster();
	~ShoutMaster();
	virtual void _InitEventSystem();
	virtual void _WaitNextEvent(float *x, float *y);
	virtual void PollEvents();
private:
	shout_t *_shout;
};

#endif /* SHOUTEVENTROUTER_H_ */
