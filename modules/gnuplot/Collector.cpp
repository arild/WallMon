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
#include "WallView.h"

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
	if (++_numSamples == get_num_samples(_sampleIntervalIndex)) {
		_sampleIntervalIndex += 1;
		_numSamples = 0;

		// Change frequency if we are not on last sample
		if (_sampleIntervalIndex != NUM_SAMPLE_INTERVALS) {
			LOG(INFO) << "changing sample frequency from " <<
					get_frequency_in_msec(_sampleIntervalIndex - 1) << " to "
					<< get_frequency_in_msec(_sampleIntervalIndex);
			ctx->sampleFrequencyMsec = get_frequency_in_msec(_sampleIntervalIndex);
		}
	}
}

extern "C" ProcessCollector *create_collector()
{
	ProcessCollector *p = new ProcessCollector(new Controller);

	if (System::IsRocksvvCluster()) {
		vector<string> servers = WallView(0,0,7,4).GetGrid();
		//vector<string> servers = WallView(0, 0, 6, 1).GetGrid();
		p->context->AddServers(servers);
		p->context->AddServer("rocksvv.cs.uit.no");
	}
	else
		p->context->AddServer("arild.dyndns.tv");

	p->context->key = KEY;
	p->context->sampleFrequencyMsec = get_frequency_in_msec(0);
	p->context->includeStatistics = true;
	p->filter->set_processname("");
	p->filter->set_pid(0);
	p->filter->set_usercpuutilization(0);
	p->filter->set_systemcpuutilization(0);
	p->filter->set_memorybytes(0);
	p->filter->set_networkinbytes(0);
	p->filter->set_networkoutbytes(0);
	return p;
}

extern "C" void destroy_collector(ProcessCollector *p)
{
	delete p;
}
