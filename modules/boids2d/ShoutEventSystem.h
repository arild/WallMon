/*
 * ShoutEventSystem.h
 *
 *  Created on: Jun 10, 2011
 *      Author: arild
 */

#ifndef SHOUTEVENTSYSTEM_H_
#define SHOUTEVENTSYSTEM_H_

#include <shout/event-types/touch-events.h>
#include <shout/event-types/shout-std-events.h>
#include "touchmanager/STouchManager.h"
#include "EventSystemBase.h"


class ShoutEventSystem : public EventSystemBase {
public:
	ShoutEventSystem();
	virtual ~ShoutEventSystem();
	virtual bool InitEventSystem();
	virtual void PollEvents();
	virtual void WaitAndHandleNextEvent();
private:
	shout_t *_shout;
};

#endif /* SHOUTEVENTSYSTEM_H_ */
