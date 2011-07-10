/*
 * Collector.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include "System.h"
#include "ProcessCollector.h"
#include "Common.h"

#define SERVER				"129.242.19.56"//"localhost"
using namespace std;

class Controller: public IProcessCollectorController {
public:
	Controller();
	virtual void AfterSample(Context *ctx);
private:
	int _numSamples, _sampleIntervalIndex;
};

Controller::Controller()
{
	_numSamples = 0;
	_sampleIntervalIndex = 0;
}

void Controller::AfterSample(Context *ctx)
{
	if (_sampleIntervalIndex == NUM_SAMPLE_INTERVALS) {
		LOG(INFO) << "gnuplot collector done, exiting";
		sleep(2); // make sure data is sent before termination
		exit(0);
	}
	_numSamples += 1;
//	LOG(INFO) << _numSamples << "/" << get_num_samples(_sampleIntervalIndex);
	if (_numSamples == get_num_samples(_sampleIntervalIndex)) {
		_sampleIntervalIndex += 1;
		_numSamples = 0;

		// Change frequency if we are not on last sample
		if (_sampleIntervalIndex != NUM_SAMPLE_INTERVALS) {
			LOG(INFO) << "changing sample frequency from " << get_frequency_in_msec(
					_sampleIntervalIndex - 1) << " to " << get_frequency_in_msec(
					_sampleIntervalIndex);
			ctx->sampleFrequencyMsec = get_frequency_in_msec(_sampleIntervalIndex);
		}
	}
}

extern "C" ProcessCollector *create_collector()
{
	ProcessCollector *p = new ProcessCollector(new Controller);

	//	list<string> processNames;
	//	processNames.push_back(WALLMON_DAEMON);
	//	processNames.push_back(WALLMON_SERVER);
	//	p->SetProcesses(processNames);

	p->context->key = KEY;
	p->context->AddServer(SERVER);
	p->context->sampleFrequencyMsec = get_frequency_in_msec(0);
	p->context->includeStatistics = true;
	p->filter->set_processname("");
	p->filter->set_pid(0);
	p->filter->set_usercpuutilization(0);
	p->filter->set_systemcpuutilization(0);
	p->filter->set_memoryutilization(0);
	p->filter->set_networkinbytes(0);
	p->filter->set_networkoutbytes(0);
	return p;
}

extern "C" void destroy_collector(ProcessCollector *p)
{
	delete p;
}
