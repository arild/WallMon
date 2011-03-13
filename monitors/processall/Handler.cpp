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
double totalUpdateTime = 0;
int numUniqueProcesses = 0;
int numUniqueHostNames = 0;

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
		string procKey =
				ProcNameStat::CreateProcMapKey(_message->hostname(), processMessage->pid());

		// Retrieve process name statistics
		if (_processNameMap->count(processName) == 0)
			// Process name not registered
			(*_processNameMap)[processName] = new ProcNameStat();
		ProcNameStat *procNameStat = (*_processNameMap)[processName];
		numUniqueProcesses = procNameStat->procMap->size();

		// Retrieve per process statistics
		ProcStat *procStat = procNameStat->Get(procKey);

		// Maintain aggregated statistics at various levels:
		// Per process name
		procNameStat->totalUserCpuLoad -= procStat->userCpuLoad;
		procNameStat->totalSystemCpuLoad -= procStat->systemCpuLoad;

		// Per unique process
		procStat->userCpuLoad = processMessage->usercpuload();
		procStat->systemCpuLoad = processMessage->systemcpuload();

		// Per Process name
		procNameStat->totalUserCpuLoad += procStat->userCpuLoad;
		procNameStat->totalSystemCpuLoad += procStat->systemCpuLoad;
	}

	totalBytesReceived += length;
	numMessagesReceived += 1;
	totalUpdateTime += _message->updatetime();

	LOG_EVERY_N(INFO, 50) << "--- Statistics ---";
	LOG_EVERY_N(INFO, 50) << "Num unique process names: " << _processNameMap->size();
	LOG_EVERY_N(INFO, 50) << "Num unique processes    : " << numUniqueProcesses;
	LOG_EVERY_N(INFO, 50) << "Average message size    : " << totalBytesReceived
				/ numMessagesReceived;
	LOG_EVERY_N(INFO, 50)
		<< "Average update time     : " << totalUpdateTime / (double) numMessagesReceived;

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
	//BarData barData[NUM_PROCESSES_TO_DISPLAY + 1];
	vector<BarData *> heap;
	struct MinHeapCompare cmp;

	double totalUserCpuLoad = 0.;
	double totalSystemCpuLoad = 0.;

	// Find the n processes with the most load
	BOOST_FOREACH (ProcNameMap::value_type &i, *_processNameMap)
	{
		string processName = i.first;
		ProcNameStat *procNameStat = i.second;

		totalUserCpuLoad += procNameStat->totalUserCpuLoad;
		totalSystemCpuLoad += procNameStat->totalSystemCpuLoad;

		// -1 since "Others" process category will be included
		if (heap.size() < (NUM_PROCESSES_TO_DISPLAY - 1)) {
			BarData *barData = new BarData(processName, procNameStat->totalUserCpuLoad,
					procNameStat->totalSystemCpuLoad);
			heap.push_back(barData);
			push_heap(heap.begin(), heap.end(), cmp);
		} else {
			//BarData current(processName, procStatSum->totalCpuLoad);
			BarData *min = heap.front();
			double totalCpuLoad = procNameStat->totalUserCpuLoad
					+ procNameStat->totalSystemCpuLoad;
			double totalCpuLoadCurrent = min->user + min->system;

			// Check if minimum element should be replaced
			if (totalCpuLoad > totalCpuLoadCurrent) {
				pop_heap(heap.begin(), heap.end(), cmp);
				min->label = processName;
				min->user = procNameStat->totalUserCpuLoad;
				min->system = procNameStat->totalSystemCpuLoad;
				push_heap(heap.begin(), heap.end(), cmp);
			}
		}
	}

	// Prepare data: relative share and sorted order
	double othersUserCpuLoad = totalUserCpuLoad;
	double othersSystemCpuLoad = totalSystemCpuLoad;
	double totalCpuLoad = totalUserCpuLoad + (double)totalSystemCpuLoad;

	BOOST_FOREACH (BarData *barData, heap)
	{
		othersUserCpuLoad -= barData->user;
		othersSystemCpuLoad -= barData->system;

		// Convert to relative shares
		barData->user = (barData->user / (double)totalCpuLoad) * 100.;
		barData->system = (barData->system / (double)totalCpuLoad) * 100.;
	}

	double relativeUser = (othersUserCpuLoad / (double)totalCpuLoad) * 100.;
	double relativeSystem = (othersSystemCpuLoad / (double)totalCpuLoad) * 100.;
	heap.push_back(new BarData("others", relativeUser, relativeSystem));
	sort(heap.begin(), heap.end(), cmp);

	//LOG(INFO) << "Top Processes:";
	//for (int i = 0; i < heap.size(); i++)
	//	LOG(INFO) << heap[i]->label << " -> " << heap[i]->GetValues();

	double utilization = totalCpuLoad / (double)2900.;
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
