/*
 * Handler.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include <boost/foreach.hpp>
#include <algorithm>
#include <vector>
#include "Handler.h"
#include "System.h"
#include "unistd.h"

#define KEY						"PROCESS_ALL_MON"
#define MESSAGE_BUF_SIZE		1024 * 1000
#define SAMPLE_FREQUENCY_MSEC 	1000
#define FRAMES_PER_SEC			2
#define NUM_PROCESSES_TO_DISPLAY	8

int numMessagesReceived = 0;
int totalBytesReceived = 0;
int numProcessMessagesReceived = 0;
double totalSampleTime = 0;
double totalSampleFrequency = 0;
int numUniqueProcesses = 0;
int numUniqueHostNames = 0;
double timestampPreviousMsec = 0.;
double timestampNowMsec = 0.;
double timestampShowStatisticsMsec = 0.;

void Handler::OnInit(Context *ctx)
{
	ctx->key = KEY;
	_message = new ProcessesMessage();
	_minHeap = new vector<double> ();

	_processNameMap = new ProcNameMap();
	_timestamp = System::GetTimeInSec();
#ifdef GRAPHICS
	LOG(INFO) << "Setting up graphics";
	_barChart = new BarChart(NUM_PROCESSES_TO_DISPLAY);
#endif
}

void Handler::OnStop()
{
	delete _message;
}

void Handler::Handle(WallmonMessage *msg)
{
	const char *data = msg->data().c_str();
	int length = msg->data().length();
	if (_message->ParseFromArray(data, length) == false)
		LOG(FATAL) << "Protocol buffer parsing failed: ";

	// Go through all process specific messages and read statistics
	for (int i = 0; i < _message->processmessage_size(); i++) {
		ProcessesMessage::ProcessMessage *processMessage = _message->mutable_processmessage(i);

		// Lookup keys
		string processName = processMessage->processname();
		string procKey = ProcNameStat::CreateProcMapKey(msg->hostname(), processMessage->pid());

		// Retrieve process name statistics
		if (_processNameMap->count(processName) == 0)
			// Process name not registered
			(*_processNameMap)[processName] = new ProcNameStat();
		ProcNameStat *procNameStat = (*_processNameMap)[processName];

		// Retrieve per process statistics
		if (procNameStat->Has(procKey) == false)
			numUniqueProcesses += 1;
		ProcStat *procStat = procNameStat->Get(procKey);

		// Maintain aggregated statistics at various levels:
		// Per process name
		procNameStat->userCpuLoad -= procStat->userCpuLoad;
		procNameStat->systemCpuLoad -= procStat->systemCpuLoad;

		// Per unique process
		procStat->userCpuLoad = processMessage->usercpuload();
		procStat->systemCpuLoad = processMessage->systemcpuload();

		// Per Process name
		procNameStat->userCpuLoad += procStat->userCpuLoad;
		procNameStat->systemCpuLoad += procStat->systemCpuLoad;
	}

	totalBytesReceived += length;
	numMessagesReceived += 1;
	totalSampleTime += msg->sampletimemsec();
	totalSampleFrequency += msg->samplefrequencymsec();
	timestampPreviousMsec = timestampNowMsec;
	timestampNowMsec = msg->timestampmsec();
	if (timestampPreviousMsec == 0.0)
		timestampPreviousMsec = timestampNowMsec;

	if (timestampNowMsec - (double)timestampShowStatisticsMsec > 1000.0) {
		char buf[100];
		LOG(INFO) << "--- Statistics ---";
		LOG(INFO) << "Num unique process names: " << _processNameMap->size();
		LOG(INFO) << "Num unique processes    : " << numUniqueProcesses;
		LOG(INFO) << "Average message size    : " << totalBytesReceived / numMessagesReceived;
		sprintf(buf, "%.4f msec", totalSampleTime / (double) numMessagesReceived);
		LOG(INFO) << "Average sample time     : " << buf;
		sprintf(buf, "%.4f msec", totalSampleFrequency / (double) numMessagesReceived);
		LOG(INFO) << "Average sample frequency: " << buf;
		sprintf(buf, "%.4f msec", msg->samplefrequencymsec());
		LOG(INFO) << "Last sample frequency   : " << buf;
		LOG(INFO) << "Time since last sample  : " << timestampNowMsec - (double)timestampPreviousMsec;
		sprintf(buf, "%.4f msec", msg->scheduledriftmsec());
		LOG(INFO) << "Scheduler drift         : " << buf;
		LOG(INFO) << "Total num samples       : " << numMessagesReceived;
		timestampShowStatisticsMsec = timestampNowMsec;
	}
#ifdef GRAPHICS
	double timestampNow = System::GetTimeInSec();
	if (timestampNow - _timestamp > (1 / (double)FRAMES_PER_SEC)) {
		_timestamp = timestampNow;
		_UpdateBarChart();
	}
#endif
}

struct MinHeapCompare {
	bool operator()(BarData * const a, BarData * const b)
	{
		//LOG(INFO) << "Compare: " << a->value << " | " << b->value;
		return (a->user + a->system) > (b->user + b->system);
	}
};

void Handler::_UpdateBarChart()
{
	vector<BarData *> heap;
	struct MinHeapCompare cmp;

	double totalUserCpuLoad = 0.;
	double totalSystemCpuLoad = 0.;
	BarData barDataArray[NUM_PROCESSES_TO_DISPLAY - 1];


	// Find the n processes with the most load
	BOOST_FOREACH (ProcNameMap::value_type &i, *_processNameMap)
	{
		string processName = i.first;
		ProcNameStat *procNameStat = i.second;

		totalUserCpuLoad += procNameStat->userCpuLoad;
		totalSystemCpuLoad += procNameStat->systemCpuLoad;

		// -1 since "Others" process category will be included
		if (heap.size() < (NUM_PROCESSES_TO_DISPLAY - 1)) {
			BarData *barData = &barDataArray[heap.size()];
			barData->label = processName;
			barData->user = procNameStat->userCpuLoad;
			barData->system = procNameStat->systemCpuLoad;
			heap.push_back(barData);
			push_heap(heap.begin(), heap.end(), cmp);
		} else {
			//BarData current(processName, procStatSum->totalCpuLoad);
			BarData *min = heap.front();
			double totalCpuLoad = procNameStat->userCpuLoad
					+ procNameStat->systemCpuLoad;
			double totalCpuLoadCurrent = min->user + min->system;

			// Check if minimum element should be replaced
			if (totalCpuLoad > totalCpuLoadCurrent) {
				pop_heap(heap.begin(), heap.end(), cmp);
				min->label = processName;
				min->user = procNameStat->userCpuLoad;
				min->system = procNameStat->systemCpuLoad;
				push_heap(heap.begin(), heap.end(), cmp);
			}
		}
	}

	// Prepare data: relative share and sorted order
	double othersUserCpuLoad = totalUserCpuLoad;
	double othersSystemCpuLoad = totalSystemCpuLoad;
	double totalCpuLoad = totalUserCpuLoad + (double) totalSystemCpuLoad;

	BOOST_FOREACH (BarData *barData, heap)
	{
		othersUserCpuLoad -= barData->user;
		othersSystemCpuLoad -= barData->system;

		// Convert to relative shares
		barData->user = (barData->user / (double) totalCpuLoad) * 100.;
		barData->system = (barData->system / (double) totalCpuLoad) * 100.;
	}

	double relativeUser = (othersUserCpuLoad / (double) totalCpuLoad) * 100.;
	double relativeSystem = (othersSystemCpuLoad / (double) totalCpuLoad) * 100.;
	BarData barData("others", relativeUser, relativeSystem);
	heap.push_back(&barData);
	sort(heap.begin(), heap.end(), cmp);

	//LOG(INFO) << "Top Processes:";
	//for (int i = 0; i < heap.size(); i++)
	//	LOG(INFO) << heap[i]->label << " -> " << heap[i]->GetValues();

	double utilization = totalCpuLoad / (double) 2900.;
	_barChart->Render(utilization, heap);
}

// class factories - needed to bootstrap object orientation with dlfcn.h
extern "C" Handler *create_handler()
{
	return new Handler;
}

extern "C" void destroy_handler(Handler *p)
{
	delete p;
}
