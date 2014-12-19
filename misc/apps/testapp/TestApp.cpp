#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>   // Declaration for exit()
#include <iostream>
#include <vector>

#include "System.h"
#include "LinuxProcessMonitor.h"
#include "WallView.h"

using namespace std;

const int MB = 1024*1024;
vector<void *> allocation;
bool memory = false;
bool systemLevel = false;

void busyWaitUserLevel(double seconds)
{
	double timestamp = System::GetTimeInSec();
	double tmp = 0;
	while (System::GetTimeInSec() - timestamp < seconds) {
		for (int i = 0; i < 9999; i++) {
			tmp += i * 4.3;
		}

		if (memory && rand() % 100 > 95) {
			for (int i = 0; i < allocation.size(); i++)
				free(allocation[i]);
			allocation.clear();
			int n = rand() % 200;
			for (int i = 0; i < n; i++) {
					allocation.push_back(malloc(MB * 40));
			}
		}
	}
}

void busyWaitSystemLevel(double seconds)
{
	double timestamp = System::GetTimeInSec();
	double tmp = 0;
	while (System::GetTimeInSec() - timestamp < seconds) {
		System::GetAllPids();
	}
}



void RunForever(double cpuLoad, LinuxProcessMonitor &m)
{
	double spinTime = cpuLoad / (double)100;
	double sleepTime = 1 - spinTime;
	double f = 0.01;
	while (true) {
        m.update();
		busyWaitUserLevel(spinTime);
		busyWaitSystemLevel(spinTime * 0.2);
		usleep(sleepTime * 1000 * 1000);
        double totalCpuLoad = m.getUserCPULoad() + m.getSystemCPULoad();
        if (totalCpuLoad > cpuLoad - 0.2) {
        	// Slow down
        	sleepTime += f;
        	spinTime -= f;
        }
        else {
        	// Speed up
        	sleepTime -= f;
        	spinTime += f;
        }
        if (sleepTime < 0)
        	sleepTime = 0;
        if (spinTime < 0)
        	spinTime = 0;
	}
}

int main(int argc, char *argv[])
{
	WallView w(1, 1, 2, 2);
	if (w.IsTileWithin())
		return 0;
	srand(getpid() + System::GetTimeInMsec());

	extern char *optarg;
	int numInstances = 1;
	int cpuLoad = 1;

	for (int c; (c = getopt(argc, argv, "n:c:ms")) != -1;) {
		switch (c)
		{
		case 'n':
			numInstances = atoi(optarg);
			break;
		case 'c':
			cpuLoad = atoi(optarg);
			break;
		case 'm':
			memory = true;
			cout << "memory" << endl;
			break;
		case 's':
			systemLevel = true;
			cout << "system" << endl;
			break;
		default:
			;
		}
	}


	pid_t pid = -1;
	for (int i = 0; i < numInstances; i++) {
		pid = fork();
		if (pid == 0) {
			srand(getpid() + System::GetTimeInMsec());
			// child
			break;
		}
		else if (pid < 0) {
			cout << "failed to fork" << endl;
			exit(1);
		}
	}

	if (pid == 0) {
		LinuxProcessMonitor m;
		m.open(getpid());
		RunForever((double)cpuLoad + (rand() % 10), m);
	}
}
