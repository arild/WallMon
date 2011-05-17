/*
 * EventHandlerBase.h
 *
 *  Created on: May 17, 2011
 *      Author: arild
 */

#ifndef EVENTHANDLERBASE_H_
#define EVENTHANDLERBASE_H_

#include <vector>
#include <boost/thread.hpp>
#include <shout/event-types/touch-events.h>
#include "Queue.h"
#include "Scene.h"

using namespace std;

class TouchEvent {
public:
	Scene *scene;
	float sceneX, sceneY, realX, realY;
	TouchEvent() {}
	virtual ~TouchEvent() {}

};

class EventHandlerBase {
public:
	EventHandlerBase();
	virtual ~EventHandlerBase();
	virtual TouchEvent _WaitNextEvent(float *x, float *y) = 0;
protected:
	Queue<TouchEvent> *GetOutputQueue();
	void Register(Scene *scene);
	boost::thread _thread;
	bool _running;
	Queue<TouchEvent> *_outputQueue;
	vector<Scene *> _scenes;
	void Start();
	void Stop();
	void _HandleEventsForever();
	void _FilterAndRouteEvent(float x, float y);
	Scene *_GlobalCoordsToScene(float x, float y);
};


#endif /* EVENTHANDLERBASE_H_ */
