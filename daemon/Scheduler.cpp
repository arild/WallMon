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

void Scheduler::RegisterColllector(IDataCollector *collector)
{
	ev_timer *timer = new ev_timer();
	ev_timer_init (timer, &Scheduler::_InterceptCallback, 3., 0.);
	try {
		collector->OnStart();
	} catch (exception &e) {
		LOG(ERROR) << "user-defined OnStart() failed: " << e.what();
	}
	try {
		LOG(INFO) << "Before GetScheduleInterval()";
		timer->repeat = collector->GetScheduleIntervalInMsec() / (double) 1000;
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
	usleep(1000 * 10000);
	while (_running) {
		LOG(INFO) << "Scheduling loop";
		ev_run(_loop, 0);
		usleep(1000 * 3000);
	}
}

void Scheduler::_InterceptCallback(struct ev_loop *loop, ev_timer *w, int revents)
{
	//TODO: Compensate for invocation (and perhaps execution delay too)
	LOG(INFO) << "**** TIMER event";
	ev_timer_again(loop, w);
	IDataCollector *collector = (IDataCollector *) w->data;
	unsigned char *data;
	int dataLength = collector->Sample(&data);
	Scheduler::_streamer->Stream(collector->GetKey(), data, dataLength);
}

