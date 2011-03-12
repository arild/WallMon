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

#include "SessionDispatcher.h"
#include "DataSink.h"
#include "SharedLibraryWatcher.h"

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

	SessionDispatcher *dispatcher = new SessionDispatcher();
	DataRouter *router = new DataRouter();
	DataSink *sink = new DataSink(router);

	router->Start();
	sink->Start();

	SharedLibraryWatcher *libraryWatcher = new SharedLibraryWatcher(router, dispatcher);

	// Block main thread and wait for termination signal
	mainThreadCondition.wait(mainThreadMutex);

	sink->Stop();
	router->Stop();

	delete dispatcher;
	delete router;
	delete sink;
	delete libraryWatcher;

	LOG(INFO) << "server successfully terminated";
}
