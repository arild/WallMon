/*
 * ProcessMonitorHandler.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include <boost/foreach.hpp>
#include <algorithm>
#include <vector>
#include "ProcessMonitor.h"
#include "System.h"
#include "unistd.h"

#define KEY		"PROCESS_ALL_MON"
#define MESSAGE_BUF_SIZE	1024 * 1000
#define SAMPLE_FREQUENCY_MSEC 	1000

#define NUM_PROCESSES_TO_DISPLAY	9

int numMessagesReceived = 0;
int totalBytesReceived = 0;
int numProcessMessagesReceived = 0;
double totalUpdateTime = 0;
int numUniqueProcesses = 0;
int numUniqueHostNames = 0;

void ProcessMonitorHandler::OnInit(Context *ctx)
{
	ctx->key = KEY;
	_message = new ProcessesMessage();
	_processNameMap = new ProcessNameMap();
	_minHeap = new vector<double> ();
	_totalCpuLoad = 0;

#ifdef GRAPHICS
	LOG(INFO) << "Setting up graphics";
	_barChart = new BarChart();
#endif
}

void ProcessMonitorHandler::OnStop()
{
	delete _message;
}

int _HeapCmpFunc(double a, double b)
{
	return b - a;
}

void ProcessMonitorHandler::Handle(void *data, int length)
{
	if (_message->ParseFromArray(data, length) == false)
		LOG(FATAL) << "Protocol buffer parsing failed: ";

	// Go through all process specific messages and read statistics
	for (int i = 0; i < _message->processmessage_size(); i++) {
		ProcessesMessage::ProcessMessage *processMessage = _message->mutable_processmessage(i);

		stringstream ss;
		ss << _message->hostname() << processMessage->pid();
		string processName = processMessage->processname();
		string hostNamePid = ss.str();

		if (_processNameMap->count(processName) == 0)
			// Process name not currently registered
			(*_processNameMap)[processName] = new ProcStatSum();

		ProcStatSum *procStatSum = (*_processNameMap)[processName];
		HostNamePidMap *hostNamePidMap = procStatSum->hostNamePidMap;

		if (hostNamePidMap->count(hostNamePid) == 0) {
			// The key hostname + pid not currently registered
			(*hostNamePidMap)[hostNamePid] = new ProcStat();
			numUniqueProcesses += 1;
		}
		ProcStat *procStat = (*hostNamePidMap)[hostNamePid];

		// Maintain aggregated statistics at various levels
		_totalCpuLoad -= procStatSum->totalCpuLoad;

		procStatSum->totalCpuLoad -= procStat->userCpuLoad;
		procStatSum->totalCpuLoad -= procStat->systemCpuLoad;

		procStat->userCpuLoad = processMessage->usercpuload();
		procStat->systemCpuLoad = processMessage->systemcpuload();

		procStatSum->totalCpuLoad += processMessage->usercpuload();
		procStatSum->totalCpuLoad += processMessage->systemcpuload();

		_totalCpuLoad += procStatSum->totalCpuLoad;
	}

	totalBytesReceived += length;
	numMessagesReceived += 1;
	totalUpdateTime += _message->updatetime();
	LOG_EVERY_N(INFO, 10) << "Num unique process names: " << _processNameMap->size();
	LOG_EVERY_N(INFO, 10) << "Average message size    : " << totalBytesReceived
				/ numMessagesReceived;
	LOG_EVERY_N(INFO, 10)
		<< "Average update time     : " << totalUpdateTime / (double) numMessagesReceived;

#ifdef GRAPHICS
	_UpdateBarChart();
#endif
}

struct MinHeapCompare {
	bool operator()(BarData * const a, BarData * const b)
	{
		//LOG(INFO) << "Compare: " << a->value << " | " << b->value;
		return a->value > b->value;
	}
};

void ProcessMonitorHandler::_UpdateBarChart()
{
	//struct std::greater<double> cmp;
	//BarData barData[NUM_PROCESSES_TO_DISPLAY + 1];
	vector<BarData *> heap;
	struct MinHeapCompare cmp;

	// Find the n processes with the most load
	BOOST_FOREACH (ProcessNameMap::value_type &i, *_processNameMap) {
		string processName = i.first;
		ProcStatSum *procStatSum = i.second;

		LOG(INFO) << "PROC NAME: " << processName;

		if (heap.size() < NUM_PROCESSES_TO_DISPLAY) {
			heap.push_back(new BarData(processName, procStatSum->totalCpuLoad));
			push_heap(heap.begin(), heap.end(), cmp);
		} else {
			//BarData current(processName, procStatSum->totalCpuLoad);
			BarData *min = heap.front();
			// Check if minimum element should be replaced
			if (procStatSum->totalCpuLoad > min->value) {//_IsGreater(&current, min)) {
				LOG(INFO) << "Replacing " << min->value << " with "
						<< procStatSum->totalCpuLoad;
				pop_heap(heap.begin(), heap.end(), cmp);
				//heap.pop_back();

				min->label = processName;
				min->value = procStatSum->totalCpuLoad;

				//heap.push_back(new BarData(processName, procStatSum->totalCpuLoad));
				push_heap(heap.begin(), heap.end(), cmp);
			}
		}
	}

	// Prepare data: relative share and sorted order
	double othersCpuLoad = _totalCpuLoad;
	double relativeShare;

	BOOST_FOREACH (BarData *barData, heap){
		othersCpuLoad -= barData->value;
		relativeShare = (barData->value / (double) _totalCpuLoad) * 100;
		barData->value = relativeShare;
	}

	relativeShare = (othersCpuLoad / (double) _totalCpuLoad) * 100;
	heap.push_back(new BarData("Others", relativeShare));
	sort(heap.begin(), heap.end(), cmp);

	LOG(INFO) << "Top Processes:";
	for (int i = 0; i < heap.size(); i++)
		LOG(INFO) << heap[i]->label << " -> " << heap[i]->value;


	_barChart->Render(heap);
}

void ProcessMonitorCollector::OnInit(Context *ctx)
{
	ctx->server = "129.242.19.57";
	ctx->key = KEY;
	ctx->sampleFrequencyMsec = SAMPLE_FREQUENCY_MSEC;

	// Create a process monitor for each pid on system
	std::list<int> *pids = System::GetAllPids();
	_monitors = new std::list<LinuxProcessMonitor *>();
	for (list<int>::iterator it = pids->begin(); it != pids->end(); it++) {
		int pid = *it;
		LinuxProcessMonitor *monitor = new LinuxProcessMonitor();
		if (monitor->open(pid) == false) {
			delete monitor;
			continue;
		}
		monitor->update();
		_monitors->push_back(monitor);
		LOG(INFO) << pid;
	}
	LOG(INFO) << "Num processes being monitored: " << _monitors->size();
	_buffer = new char[MESSAGE_BUF_SIZE];
	memset(_buffer, 0, MESSAGE_BUF_SIZE);
}

void ProcessMonitorCollector::OnStop()
{
	delete _buffer;
}

int ProcessMonitorCollector::Sample(void **data)
{

	ProcessesMessage processesMsg;
	char hostname[100];
	gethostname(hostname, 100);

	double start = System::GetTimeInSec();
	for (list<LinuxProcessMonitor *>::iterator it = _monitors->begin(); it != _monitors->end(); it++) {
		LinuxProcessMonitor *monitor = (*it);
		monitor->update();

		double userCpuLoad = monitor->getUserCPULoad();
		double systemCpuLoad = monitor->getSystemCPULoad();
		string processName = monitor->comm();

		// Remove '(' and ')' from the process name returned from comm()
		processName.erase(processName.length() - 1, 1);
		processName.erase(0, 1);

		ProcessesMessage::ProcessMessage *processMsg = processesMsg.add_processmessage();
		processMsg->set_processname(processName);
		processMsg->set_pid(monitor->pid());
		processMsg->set_usercpuload(userCpuLoad);
		processMsg->set_systemcpuload(systemCpuLoad);
	}

	processesMsg.set_updatetime(System::GetTimeInSec() - (double) start);
	processesMsg.set_hostname(hostname);

	LOG(INFO) << "Serializing num bytes: " << processesMsg.ByteSize();
	if (processesMsg.SerializeToArray(_buffer, MESSAGE_BUF_SIZE) != true)
		LOG(ERROR) << "Protocol buffer serialization failed";
	*data = _buffer;
	return processesMsg.ByteSize();
}

// class factories - needed to bootstrap object orientation with dlfcn.h
extern "C" ProcessMonitorHandler *create_handler()
{
	return new ProcessMonitorHandler;
}

extern "C" void destroy_handler(ProcessMonitorHandler *p)
{
	delete p;
}

extern "C" ProcessMonitorCollector *create_collector()
{
	return new ProcessMonitorCollector;
}

extern "C" void destroy_collector(ProcessMonitorCollector *p)
{
	delete p;
}
