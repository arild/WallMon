
#include <google/profiler.h>
#include <iostream>
#include <stdlib.h>
#include "System.h"
#include "ProcessMonitoring.h"

using namespace std;

int main(int argc, char *argv[]) {

	int numSamples = 1000;
	if (argc > 1)
		numSamples = atoi(argv[1]);

	cout << "running " << numSamples << " samples" << endl << endl;

	ProcessMonitoring p;
	p.RunProtobufBenchmark();
	sleep(1);

	p.RunProcfsBenchmark(numSamples, true, true, true); // Warm-up
	sleep(1);

	// Include protobuf - minimal measurements
	ProfilerStart("procfs.prof");
	p.RunProcfsBenchmark(numSamples, false, true, false);
	ProfilerStop();
	sleep(1);

	// No protobuf - just procfs
	p.RunProcfsBenchmark(numSamples, false, false, false);
	sleep(1);

	// Measure everything
	p.RunProcfsBenchmark(numSamples, true, true, true);
}
