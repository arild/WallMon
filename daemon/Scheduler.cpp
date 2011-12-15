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
class CollectorTimerEvent {
public:
	Context *ctx; // The context which is shared and known by the collector
	IDataCollector *collector; // The collector implementation itself
	IDataCollectorProtobuf *collectorProtobuf;

	double scheduleDurationSec; // The actual time it takes for a collector to be invoked over again
	double previousTsBeforeSampleSec; // Used to calculate scheduleDurationSec
	unsigned int numSamples; // Number of times the collector has been invoked
	unsigned int numSamplesLastSecond;

	double tsNumSamplesPerSecond; // Time stamp used for calculating the number of samples for the last second

	CollectorTimerEvent()
	{
		scheduleDurationSec = -1;
		previousTsBeforeSampleSec = -1;
		numSamples = 0;
		numSamplesLastSecond = 0;
		tsNumSamplesPerSecond = ev_time();
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
		delete (CollectorTimerEvent *) timer->data;
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
	ev_timer_init(timer, &Scheduler::_TimerCallback, scheduleIntervalInSec, scheduleIntervalInSec);

	CollectorTimerEvent *event = new CollectorTimerEvent();
	event->ctx = &ctx;
	event->collector = dynamic_cast<IDataCollector *> (&monitor);
	event->collectorProtobuf = dynamic_cast<IDataCollectorProtobuf *> (&monitor);

	timer->data = (void *) event;
	ev_timer_start(_loop, timer);
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
void Scheduler::_TimerCallback(struct ev_loop *loop, ev_timer *timer, int revents)
{
	CollectorTimerEvent *event = (CollectorTimerEvent *) timer->data;
	double oldSampleFrequencyMsec = event->ctx->sampleFrequencyMsec;
	double tsBeforeSampleSec = ev_now(loop);

	WallmonMessage msg;
	msg.set_key(event->ctx->key);
	if (event->collector) {
		void *data;
		int dataLen = event->collector->Sample(&data);
		msg.set_data(data, dataLen);
	} else if (event->collectorProtobuf) {
		event->collectorProtobuf->Sample(&msg);
	} else
		LOG(FATAL) << "unknown collector type";

	if (event->ctx->sampleFrequencyMsec < 0) {
		event->ctx->sampleFrequencyMsec = 0;
		LOG(WARNING) << "negative sample frequency of key: " << event->ctx->key;
	}

	double tsAfterSampleSec = ev_time();
	double sampleDurationSec = tsAfterSampleSec - tsBeforeSampleSec;
	if (event->ctx->sampleFrequencyMsec != oldSampleFrequencyMsec) {
		// Tell libev about new frequency
		double repeat = event->ctx->sampleFrequencyMsec / (double) 1000;
		double again = repeat - sampleDurationSec; // Account for time spent during sampling
		// Re-schedules timer to trigger after 'again' seconds, and with a repeating
		// trigger value of 'repeat' seconds
		if (again < 0) {
			LOG(INFO) << "scheduler not able to keep up with sample frequency: again=" << again;
			again = 0;
		}
		ev_timer_set(timer, again, repeat); // Modifies internal values
		ev_timer_again(loop, timer); // Restarts the timer (loading new values)

		if (event->ctx->includeStatistics) {
			// Reset hz tracker
			event->numSamplesLastSecond = 0;
			event->tsNumSamplesPerSecond = tsAfterSampleSec;
		}
	}

	if (msg.has_data() == false) {
		LOG(INFO) << "data field not present in collector messsage";
		return;
	}

	if (event->ctx->includeStatistics) {
		msg.set_collectorsequencenumber(event->numSamples);
		event->numSamples += 1;
		event->numSamplesLastSecond += 1;
		msg.set_daemontimestampmsec(tsBeforeSampleSec * (double)1000);
		msg.set_hostname(System::GetHostname());
		msg.set_samplefrequencymsec(oldSampleFrequencyMsec);
		msg.set_sampledurationmsec(sampleDurationSec * (double)1000);
		if (event->previousTsBeforeSampleSec > 0) {
			double actualFrequencyMsec = (tsBeforeSampleSec - event->previousTsBeforeSampleSec) * (double)1000;
			msg.set_scheduledriftmsec(actualFrequencyMsec - (double)oldSampleFrequencyMsec);
		}
		if (event->scheduleDurationSec > 0)
			msg.set_scheduledurationmsec(event->scheduleDurationSec * (double)1000);

		double diff = tsAfterSampleSec - (double)event->tsNumSamplesPerSecond;
		if (tsAfterSampleSec - (double)event->tsNumSamplesPerSecond > 1.) {
			msg.set_actualhz(event->numSamplesLastSecond);
			event->numSamplesLastSecond = 0;
			event->tsNumSamplesPerSecond = tsAfterSampleSec;
		}
		msg.set_daemonqueuesize(Scheduler::streamer->GetNumPendingItems());
		unsigned int numConnections = Scheduler::streamer->GetNumServerConnections();
		if (numConnections > 0)
			msg.set_daemonnumserverconnections(Scheduler::streamer->GetNumServerConnections());
	}

	// Compose network message
	StreamItem &item = *new StreamItem(msg.ByteSize());
	msg.SerializeToArray(item.GetPayloadStartReference(), msg.ByteSize());

	// Associate defined destinations (socket file descriptors) with composed message
	for (int i = 0; i < event->ctx->servers.size(); i++) {
		string serverAddress = event->ctx->servers[i].get<0> ();
		int serverPort = event->ctx->servers[i].get<1> ();
		int sockfd = streamer->SetupStream(serverAddress, serverPort);
		if (sockfd > -1)
			item.serversSockFd.push_back(sockfd);
		else
			LOG(ERROR) << "invalid sockfd for: " << serverAddress;
	}

	// Queue message for transmission
	Scheduler::streamer->Stream(item);

	if (event->ctx->includeStatistics) {
		event->previousTsBeforeSampleSec = tsBeforeSampleSec; // Save timestamp until next schedule
		event->scheduleDurationSec = ev_time() - tsBeforeSampleSec;
	}
}

