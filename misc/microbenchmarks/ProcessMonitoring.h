/*
 * ProcessMonitoring.h
 *
 *  Created on: Jul 9, 2011
 *      Author: arild
 */

#ifndef PROCESSMONITORING_H_
#define PROCESSMONITORING_H_

#include "LinuxProcessMonitorLight.h"
#include <vector>

using namespace std;

class ProcessMonitoring {
public:
	ProcessMonitoring();
	virtual ~ProcessMonitoring();
	void RunProcfsBenchmark(int numSamples, bool verboseProcfs=false, bool includeProtobuf=false, bool verboseProtobuf=false);
	void RunProtobufBenchmark();
private:
	vector<LinuxProcessMonitorLight *> _monitors;

};

#endif /* PROCESSMONITORING_H_ */
