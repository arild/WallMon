/*
 * ShoutEventSystem.h
 *
 *  Created on: Jun 10, 2011
 *      Author: arild
 */

#ifndef SHOUTEVENTSYSTEM_H_
#define SHOUTEVENTSYSTEM_H_

#include <shout/event-types/touch-events.h>
#include "touchmanager/STouchManager.h"
#include "EventSystemBase.h"


class ShoutEventSystem : public EventSystemBase {
public:
	ShoutEventSystem();
	virtual ~ShoutEventSystem();
	virtual bool InitEventSystem();
	virtual void PollEvents();
	virtual void WaitAndHandleNextEvent();
	void HandleTouches(touchVector_t & down, touchVector_t & up);
private:
	shout_t *_shout;
	STouchManager *_touchManager;
};

#endif /* SHOUTEVENTSYSTEM_H_ */
