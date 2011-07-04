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
	_numSamples += 1;
	if (_numSamples == SAMPLE_INTERVALS[_sampleIntervalIndex].numSamples) {
		if (_sampleIntervalIndex == NUM_SAMPLE_INTERVALS) {
			LOG(INFO) << "gnuplot collector done, exiting";
			exit(0);
		}
		LOG(INFO) << "changing sample frequency from "
				<< SAMPLE_INTERVALS[_sampleIntervalIndex].sampleFrequency << " to "
				<< SAMPLE_INTERVALS[_sampleIntervalIndex + 1].sampleFrequency;
		_sampleIntervalIndex += 1;
		ctx->sampleFrequencyMsec = SAMPLE_INTERVALS[_sampleIntervalIndex].sampleFrequency;
		_numSamples = 0;
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
	p->context->sampleFrequencyMsec = SAMPLE_INTERVALS[0].sampleFrequency;

	p->filter->set_processname("");
	p->filter->set_pid(0);
	p->filter->set_usercpuutilization(0);
	p->filter->set_systemcpuutilization(0);
	p->filter->set_memoryutilization(0);
	p->filter->set_networkinutilization(0);
	p->filter->set_networkoututilization(0);
	return p;
}

extern "C" void destroy_collector(ProcessCollector *p)
{
	delete p;
}
