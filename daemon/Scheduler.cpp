/*
 * Scheduler.cpp
 *
 *  Created on: Jan 28, 2011
 *      Author: arild
 */

#include <sys/time.h>
#include <string>
#include <unistd.h>
#include <iostream>
#include <glog/logging.h>
#include "Scheduler.h"

using namespace std;

Streamer *Scheduler::_streamer;

ev_tstamp last_ts;

typedef struct TimerEventContext {
	IDataCollector *collector;
	Streamer *streamer;
} TimerEventContext_t;


Scheduler::Scheduler()
{
	_loop = ev_default_loop(0);
	LOG(INFO) << "LOOP CREATED";
}

Scheduler::~Scheduler()
{
}

void Scheduler::SetStreamer(Streamer *streamer)
{
	Scheduler::_streamer = streamer;
}

/**
 * Registers a given collector with an event timer that is
 * inserted into the global event loop.
 */
void Scheduler::RegisterColllector(IDataCollector *collector)
{
	ev_timer *timer = new ev_timer();
	try {
		// Adhere to interface contract
		collector->OnStart();
	} catch (exception &e) {
		LOG(ERROR) << "user-defined OnStart() failed: " << e.what();
	}
	try {
		LOG(INFO) << "Before GetScheduleInterval()";
		double scheduleIntervalInSec = collector->GetScheduleIntervalInMsec() / (double) 1000;
		ev_timer_init (timer, &Scheduler::_InterceptCallback, scheduleIntervalInSec, 0.);
		timer->repeat = scheduleIntervalInSec;
	} catch (exception &e) {
		LOG(ERROR) << "user-defined GetScheduleIntervalInMsec() failed: " << e.what();
	}
	timer->data = (void *) collector;
	ev_timer_again(_loop, timer);
	LOG(INFO) << "register done";
}

void Scheduler::Start()
{
	_running = true;
	_thread = boost::thread(&Scheduler::_ScheduleForever, this);
	LOG(INFO) << "Scheduler running";
}

void Scheduler::Stop()
{
	_running = false;
	_thread.join();
	LOG(INFO) << "Scheduler terminated";
}

void Scheduler::_ScheduleForever()
{
	while (_running) {
		LOG(INFO) << "Scheduling loop";
		// Blocks as long as there are one or more timers registered.
		// In order to terminate the loop, all registered timers must be removed.
		ev_run(_loop, 0);
		usleep(1000 * 500); // 500 Msec
	}
}

void Scheduler::_InterceptCallback(struct ev_loop *loop, ev_timer *w, int revents)
{
	ev_timer_again(loop, w);
	double start = ev_time();
	double now_ts = ev_now(loop);
	//LOG(INFO) << "Last: " << last_ts << " Now: " << now_ts << " Diff: " << now_ts - last_ts;
	//LOG(INFO) << "timer->at: " << w->at << " timer->repeat: " << w->repeat;
	IDataCollector *collector = (IDataCollector *) w->data;

	unsigned char *buf = new unsigned char[1024*10];
	unsigned char *tmp;

	StreamItem_t item = *new StreamItem();
	int dataLen = collector->Sample((unsigned char **)&tmp);

	string key = collector->GetKey();
	int keyLen = key.length() + 1;
	memcpy(buf, key.c_str(), keyLen);
	memcpy(buf + keyLen, tmp, dataLen);

	item.length = keyLen + dataLen;
	item.data = buf;

	LOG(INFO) << "Item ready, streaming it";
	Scheduler::_streamer->Stream(item);
	LOG(INFO) << "collector execution time: " << ev_time() - start << " msec";
	LOG(INFO) << "timer schedule offset: " << now_ts - last_ts << " msec";
	last_ts = now_ts;
}

