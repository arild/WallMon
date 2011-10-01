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
#include "Stat.h"


using namespace std;

class ProcessMonitoring {
public:
	Stat<double> protobufPackSamples, protobufSerializationSamples, procfsReadSamples, procfsParseSamples;
	ProcessMonitoring();
	virtual ~ProcessMonitoring();
	void RunProcfsBenchmark(int numSamples);
	void RunProtobufBenchmark();
private:
	vector<LinuxProcessMonitorLight *> _monitors;

};

#endif /* PROCESSMONITORING_H_ */
