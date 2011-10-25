
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
	p.RunProcfsBenchmark(numSamples);
	p.RunProtobufBenchmark();
}
