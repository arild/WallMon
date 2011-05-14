/*
 * ShoutEventRouter.h
 *
 *  Created on: May 13, 2011
 *      Author: arild
 */

#ifndef SHOUTEVENTROUTER_H_
#define SHOUTEVENTROUTER_H_

#include <vector>
#include <boost/tuple/tuple.hpp>
#include <boost/thread.hpp>
#include <shout/event-types/touch-events.h>
#include "Queue.h"
#include "Scene.h"

using namespace std;

class TouchEvent {
public:
	Scene *scene;
	float x, y;
	TouchEvent(Scene *scene_, float x_, float y_) : scene(scene_), x(x_), y(y_) {};
};

class ShoutMaster {
public:
	ShoutMaster();
	virtual ~ShoutMaster();
	void Start();
	void Stop();
	Queue<TouchEvent> *GetOutputQueue();
private:
	boost::thread _thread;
	bool _running;
	Queue<TouchEvent> *_outputQueue;
	void _HandleShoutEventsForever();
	void _ParseShoutEvent(shout_event_t *event);
	Scene *_GlobalCoordsToScene(float x, float y);
};

#endif /* SHOUTEVENTROUTER_H_ */
