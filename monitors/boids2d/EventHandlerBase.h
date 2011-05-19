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
	boost::thread _thread;
	bool _running;
	Queue<TouchEvent> *_outputQueue;
	vector<Scene *> _scenes;

	EventHandlerBase();
	virtual ~EventHandlerBase();
	virtual void _InitEventSystem() = 0;
	virtual void _WaitNextEvent(float *x, float *y) = 0;
	virtual void PollEvents() = 0;


	void _HandleEventsForever();
	void _FilterAndRouteEvent(float x, float y);
	Scene *_GlobalCoordsToScene(float x, float y);
	void Start();
	void Stop();
	Queue<TouchEvent> *GetOutputQueue();
	void Register(Scene *scene);
};


#endif /* EVENTHANDLERBASE_H_ */
