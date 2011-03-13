#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <list>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <ev++.h>
#include "Wallmon.h"
#include "Wallmon.pb.h"
#include "Streamer.h"

using namespace std;

typedef list<ev_timer *> TimerContainer;

class Scheduler {
public:
	Scheduler(Streamer *streamer);
	virtual ~Scheduler();
	void Start();
	void Stop();
	void RegisterColllector(IBase &collector, Context *ctx);
	static Streamer *streamer;
private:
	boost::thread _thread;
	bool _running;
	struct ev_loop *_loop;
	boost::condition _loopCondition;
	boost::mutex _loopMutex;
	TimerContainer *_timers;

	void _ScheduleForever();
	static void _TimerCallback(struct ev_loop *loop, ev_timer *w, int revents);
};

#endif /* SCHEDULER_H_ */
