#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <glog/logging.h>
#include <iostream>
#include <unistd.h>

#include "SessionDispatcher.h"
#include "DataSink.h"
#include "SharedLibraryWatcher.h"

using namespace std;

bool RUNNING = true;
void sigproc(int k)
{
	RUNNING = false;
}

int main(int argc, char *argv[])
{

	google::InitGoogleLogging(argv[0]);
	signal(SIGINT, &sigproc);
	cout << "START" << endl;

	SessionDispatcher *dispatcher = new SessionDispatcher();
	DataRouter *router = new DataRouter();
	DataSink *sink = new DataSink(router);
	SharedLibraryWatcher *libraryWatcher = new SharedLibraryWatcher(router, dispatcher);

	sink->Start();
	while (RUNNING) {
		cout << "Running..." << endl;
		usleep(1000 * 1000);
	}
	sink->Stop();

	delete dispatcher;
	delete router;
	delete sink;
	delete libraryWatcher;

	return 0;
}
