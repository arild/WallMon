/**
 * @file   Main.cpp
 * @Author Arild Nilsen
 * @date   January, 2011
 *
 * Starts and terminates core modules in the server
 */

#include <signal.h>
#include <glog/logging.h>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

#include "Config.h"
#include "System.h"
#include "IMonitorManager.h"
#include "MonitorDispatcher.h"
#include "DataSink.h"

using namespace std;

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
		default:
			;
		}
	}

	DataRouter *router = new DataRouter();
	DataSink *sink = new DataSink(router);
	IMonitorManager *manager = (IMonitorManager *) router;
	MonitorDispatcher *dispatcher = new MonitorDispatcher(*manager, SERVER_MULTICAST_LISTEN_PORT);

	dispatcher->Start();
	router->Start();
	sink->Start();

	// Block main thread and wait for termination signal
	mainThreadCondition.wait(mainThreadMutex);

	dispatcher->Stop();
	sink->Stop();
	router->Stop();

	delete dispatcher;
	delete router;
	delete sink;

	LOG(INFO) << "server successfully terminated";
}
