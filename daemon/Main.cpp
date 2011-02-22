#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <glog/logging.h>

#include "Dispatcher.h"
#include "Streamer.h"
#include "System.h"

bool RUNNING = true;
void sigproc(int k)
{
	RUNNING = false;
}

int main(int argc, char *argv[])
{
	google::InitGoogleLogging(argv[0]);
	signal(SIGINT, &sigproc);
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

	Scheduler *scheduler = new Scheduler();
	Dispatcher *dispatcher = new Dispatcher(scheduler);

	scheduler->Start();
	dispatcher->Start();

	while (RUNNING) {
		LOG(INFO) << "Running...";
		usleep(1000 * 1000);
	}

	dispatcher->Stop();
	// Note: dispatcher will stop scheduler

	delete dispatcher;
	delete scheduler;
	LOG(INFO) << "Daemon successfully terminated";
}

/*
 * 	FILE *fp;
 int status;
 char path[1035];

 status = 2;

 LOG(INFO) << "Found " << 2 << " cookies";

 fp = popen("ls ~", "r");
 if (fp == NULL) {
 printf("Failed to run command\n");
 exit(0);
 }

 while (fgets(path, sizeof(path) - 1, fp) != NULL) {
 ;//printf("%s", path);
 }

 pclose(fp);
 */

