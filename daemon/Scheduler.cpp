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
#include "Streamer.h"

using namespace std;

ev_tstamp last_ts;

class CollectorEvent {
public:
	IDataCollector *collector;
	Context *ctx;
	Streamer *streamer;
};

Scheduler::Scheduler()
{
	_loop = ev_default_loop(0);
	LOG(INFO) << "LOOP CREATED";
}

Scheduler::~Scheduler()
{
}

/**
 * Registers a given collector with an event timer that is
 * inserted into the global event loop.
 */
void Scheduler::RegisterColllector(IDataCollector &collector, Streamer &streamer)
{
	ev_timer *timer = new ev_timer();
	Context *ctx = new Context();
	try {
		// Adhere to interface contract
		collector.OnInit(ctx);
	} catch (exception &e) {
		LOG(ERROR) << "user-defined OnStart() failed: " << e.what();
	}
	LOG(INFO) << "Before GetScheduleInterval()";
	double scheduleIntervalInSec = ctx->sampleFrequencyMsec / (double) 1000;
	ev_timer_init (timer, &Scheduler::_InterceptCallback, scheduleIntervalInSec, 0.);
	timer->repeat = scheduleIntervalInSec;

	CollectorEvent *event = new CollectorEvent();
	event->collector = &collector;
	event->ctx = ctx;
	event->streamer = &streamer;

	timer->data = (void *) event;
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
	CollectorEvent *event = (CollectorEvent *) w->data;

	char *buf = new char[1024 * 10];
	void *tmp;

	StreamItem_t item = *new StreamItem();
	int dataLen = event->collector->Sample(&tmp);

	string key = event->ctx->key;
	int keyLen = key.length() + 1;
	memcpy(buf, key.c_str(), keyLen);
	memcpy(buf + keyLen, tmp, dataLen);

	item.length = keyLen + dataLen;
	item.data = (void *) buf;

	LOG(INFO) << "Item ready, streaming it. Key: " << key;
	event->streamer->Stream(item);
	//LOG(INFO) << "collector execution time: " << ev_time() - start << " msec";
	//LOG(INFO) << "timer schedule offset: " << now_ts - last_ts << " msec";
	last_ts = now_ts;
}

