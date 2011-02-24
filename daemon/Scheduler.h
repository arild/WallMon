/*
 * Scheduler.h
 *
 *  Created on: Jan 28, 2011
 *      Author: arild
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <boost/thread.hpp>
#include <ev++.h>
#include "Wallmon.h"
#include "Streamer.h"
#include <boost/unordered_map.hpp>

//using std::string;
//typedef boost::unordered_map<string, IDataHandler *> handlerMap;

class Scheduler {
public:
	Scheduler(Streamer *streamer);
	virtual ~Scheduler();
	void RegisterColllector(IDataCollector &collector, Context *ctx);
	void Start();
	void Stop();
	static Streamer *streamer;
private:
	boost::thread _thread;
	bool _running;
	struct ev_loop *_loop;
	void _ScheduleForever();
	static void _InterceptCallback(struct ev_loop *loop, ev_timer *w, int revents);
};

#endif /* SCHEDULER_H_ */
