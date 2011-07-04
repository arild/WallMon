/**
 * @file   Scheduler.cpp
 * @Author Arild Nilsen
 * @date   January, 2011
 *
 * Schedules user-defined collectors according to specified intervals.
 */

#include <glog/logging.h>
#include <vector>
#include <string>
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include "Scheduler.h"
#include "Streamer.h"
#include "System.h"
#include "Config.h"
#include "stubs/Wallmon.pb.h"

using namespace boost::tuples;

/**
 * Container for user-data associated with timers.
 *
 * Timers in libev have a special attribute where the user (programmer)
 * can store arbitrary data. The timers used here store an instance of this class.
 */
class CollectorEvent {
public:
	Context *ctx; // The context which is shared and known by the collector
	IDataCollector *collector; // The collector implementation itself
	IDataCollectorProtobuf *collectorProtobuf;

	double timestampLastScheduleMsec;
	double scheduleDriftinMsec; // Accumulated drift for when collector is scheduled
	unsigned int numSamples; // Number of times the collector has been invoked

	CollectorEvent()
	{
		timestampLastScheduleMsec = 0.;
		scheduleDriftinMsec = 0.;
		numSamples = 0;
	}
};

Streamer *Scheduler::streamer;

Scheduler::Scheduler(Streamer *streamer)
{
	_loop = ev_default_loop(0);
	_timers = new TimerContainer();
	Scheduler::streamer = streamer;
}

Scheduler::~Scheduler()
{
	ev_loop_destroy(_loop);
	delete _timers;
}

void Scheduler::Start()
{
	_running = true;
	_thread = boost::thread(&Scheduler::_ScheduleForever, this);
	LOG(INFO) << "Scheduler started";
}

void Scheduler::Stop()
{
	LOG(INFO) << "stopping Scheduler...";
	_running = false;
	_loopCondition.notify_one(); // In case no timers are registered
	// Stop all timers, this will cause the event loop to terminate
	TimerContainer::iterator it;
	for (it = _timers->begin(); it != _timers->end(); it++) {
		ev_timer *timer = *it;
		ev_timer_stop(_loop, timer);

		// Release memory allocated in RegisterColllector()
		delete (CollectorEvent *)timer->data;
		delete timer;
	}
	_thread.join(); // Wait for event loop to terminate
	LOG(INFO) << "Scheduler stopped";
}

/**
 * Starts the event loop that schedules timer events.
 *
 * The ev_run() call blocks as long as there are one or more timers registered.
 * In order to avoid spinning when there are no timers registered, the RegisterCollector()
 * call will notify a shared condition variable whenever a new collector/timer is registered.
 */
void Scheduler::_ScheduleForever()
{
	LOG(INFO) << "Scheduler started and serving requests";
	while (_running) {
		_loopCondition.wait(_loopMutex);
		ev_run(_loop, 0);
	}
}

/**
 * Registers a given collector with an event timer that is
 * inserted into the global event loop.
 */
void Scheduler::Register(IBase & monitor, Context & ctx)
{
	ev_timer *timer = new ev_timer();

	double scheduleIntervalInSec = ctx.sampleFrequencyMsec / (double) 1000;
	ev_timer_init(timer, &Scheduler::_TimerCallback, scheduleIntervalInSec, 0.);
	timer->repeat = scheduleIntervalInSec;

	CollectorEvent *event = new CollectorEvent();
	event->ctx = &ctx;
	event->collector = dynamic_cast<IDataCollector *> (&monitor);
	event->collectorProtobuf = dynamic_cast<IDataCollectorProtobuf *> (&monitor);

	timer->data = (void *) event;
	ev_timer_again(_loop, timer);
	_loopCondition.notify_one();

	// Book-keep for cleanup
	_timers->push_back(timer);
}

void Scheduler::Event(IBase & monitor, char *msg)
{
}

void Scheduler::Remove(IBase & monitor)
{
}

/**
 * This method serves as a callback when a timer is scheduled.
 */
void Scheduler::_TimerCallback(struct ev_loop *loop, ev_timer *w, int revents)
{
	double timestampBeforeSampleInMsec = System::GetTimeInMsec();
	ev_timer_again(loop, w); // Re-schedule collector

	CollectorEvent *event = (CollectorEvent *) w->data;
	WallmonMessage msg;
	msg.set_key(event->ctx->key);

	if (event->collector) {
		void *data;
		int dataLen = event->collector->Sample(&data);
		msg.set_data(data, dataLen);
	} else if (event->collectorProtobuf) {
		event->collectorProtobuf->Sample(&msg);
	}
	else
		LOG(FATAL) << "unknown collector type";

	if (event->ctx->sampleFrequencyMsec < 0) {
		event->ctx->sampleFrequencyMsec = 0;
		LOG(WARNING) << "negative sample frequency of key: " << event->ctx->key;
	}
	double timestampAfterSampleInMsec = System::GetTimeInMsec();

	// Update local statistics
	if (event->timestampLastScheduleMsec != 0.) {
		double actualFrequencyMsec = (double)(timestampBeforeSampleInMsec - (double)event->timestampLastScheduleMsec);
		event->scheduleDriftinMsec = (actualFrequencyMsec - event->ctx->sampleFrequencyMsec);
	}
	event->timestampLastScheduleMsec = timestampBeforeSampleInMsec; // Save timestamp until next schedule
	event->numSamples += 1;

	w->repeat = event->ctx->sampleFrequencyMsec / (double)1000.;

	// Statistics sent to server
	msg.set_timestampmsec(timestampAfterSampleInMsec);
	msg.set_hostname(System::GetHostname());
	msg.set_samplefrequencymsec(event->ctx->sampleFrequencyMsec);
	msg.set_sampletimemsec(timestampAfterSampleInMsec - timestampBeforeSampleInMsec);
	msg.set_scheduledriftmsec(event->scheduleDriftinMsec);

	// Compose network message
	StreamItem &item = *new StreamItem(msg.ByteSize());
	msg.SerializeToArray(item.GetPayloadStartReference(), msg.ByteSize());
	for (int i = 0; i < event->ctx->servers.size(); i++) {
		string serverAddress = event->ctx->servers[i].get<0>();
		int serverPort = event->ctx->servers[i].get<1>();
		LOG_EVERY_N(INFO, 1000) << "Streaming to: " << serverAddress << " | " << serverPort;
		int sockfd = streamer->SetupStream(serverAddress, serverPort);
		if (sockfd > -1)
			item.serversSockFd.push_back(sockfd);
		else
			LOG(ERROR) << "invalid sockfd for: " << serverAddress;
	}
	// Queue message for transmission
	Scheduler::streamer->Stream(item);
}

