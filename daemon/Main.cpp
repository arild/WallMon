/**
 * @file   Main.cpp
 * @Author Arild Nilsen
 * @date   January, 2011
 *
 * Starts and terminates core modules in the daemon
 */

#include <stdio.h>
#include <signal.h>
#include <glog/logging.h>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

#include "IMonitorManager.h"
#include "MonitorDispatcher.h"
#include "Streamer.h"
#include "Scheduler.h"
#include "System.h"
#include "Config.h"

boost::condition mainThreadCondition;
boost::mutex mainThreadMutex;

void sigproc(int signal)
{
	LOG(INFO) << "signal catched: " << signal;
	mainThreadCondition.notify_one();
}

int main(int argc, char *argv[])
{
	google::InitGoogleLogging(argv[0]);
	signal(SIGINT, &sigproc); // CTRL+C
	signal(SIGTERM, &sigproc); // pkill -SIGTERM wallmond

	for (int c; (c = getopt(argc, argv, "d")) != -1;) {
		switch (c)
		{
		case 'd':
			LOG(INFO) << "Daemonizing...";
			System::Daemonize();
			break;
		case '?':
			if (optopt == 'c')
				fprintf(stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint(optopt))
				fprintf(stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
			return 1;
		default:
			;
		}
	}

	// Create and start threads based on dependencies between each other
	Streamer *streamer = new Streamer();
	streamer->Start();

	Scheduler *scheduler = new Scheduler(streamer);
	scheduler->Start();

	IMonitorManager *manager = (IMonitorManager *) scheduler;
	MonitorDispatcher *dispatcher = new MonitorDispatcher(*manager, DAEMON_MULTICAST_LISTEN_PORT);
	dispatcher->Start();

	// Block main thread and wait for termination signal
	mainThreadCondition.wait(mainThreadMutex);

	// Stop in reverse order
	dispatcher->Stop();
	scheduler->Stop();
	streamer->Stop();

	delete dispatcher;
	delete scheduler;
	delete streamer;

	LOG(INFO) << "daemon successfully terminated";
}
