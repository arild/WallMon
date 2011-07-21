#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "LinuxProcessMonitor.h"
#include <sys/types.h>
#include <unistd.h>

bool running;


void worker_thread()
{
	int i = 1;
	float j = 2.4;
	while (running)
		j = (((i++) * 2) / (float)4) * j;
	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
	pthread_t thread;
	pthread_attr_t attr;
	int i;

	running = true;
	int num_threads = 2;
	if (argc > 1)
		num_threads = atoi(argv[1]);

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	for (i = 0; i < num_threads; i++) {
		if (pthread_create(&thread, &attr, (void *(*)(void *))&worker_thread, NULL) != 0)
			printf("failed creating thread");
	}

	LinuxProcessMonitor m;
	m.open(getpid());
	printf("Dispatched %d threads\n", num_threads);
	int num_sec = 0;
	while (num_sec < 10) {
		m.update();
		sleep(1);
		double load = m.getUserCPULoad() + m.getSystemCPULoad();
		if (num_sec++ > 0)
			printf("Total process cpu load: %.2f\n", load);
	}
	running = false;
	printf("Done\n");
}

