/*
 * Collector.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include <boost/foreach.hpp>

#include "Collector.h"
#include "System.h"
#include "unistd.h"

#define KEY		"PROCESS_ALL_MON"
#define MESSAGE_BUF_SIZE	(1024 * 1000) * 5
#define SAMPLE_FREQUENCY_MSEC 	1000


void Collector::OnInit(Context *ctx)
{
	ctx->server = "129.242.19.57";
	ctx->key = KEY;
	ctx->sampleFrequencyMsec = SAMPLE_FREQUENCY_MSEC;
	_ctx = ctx;
	// Create a process monitor for each pid on system
	std::vector<int> *pids = System::GetAllPids();
	_monitors = new std::vector	<LinuxProcessMonitor *>();
	for (vector<int>::iterator it = pids->begin(); it != pids->end(); it++) {
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

void Collector::OnStop()
{
	delete _buffer;
}

void Collector::Sample(WallmonMessage *msg)
{
	ProcessesMessage processesMsg;
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

	if (processesMsg.SerializeToArray(_buffer, MESSAGE_BUF_SIZE) != true)
		LOG(ERROR) << "Protocol buffer serialization failed";
	msg->set_data(_buffer, processesMsg.ByteSize());
//	if (_ctx->sampleFrequencyMsec > 0)
//		_ctx->sampleFrequencyMsec -= 1;
}

extern "C" Collector *create_collector()
{
	return new Collector;
}

extern "C" void destroy_collector(Collector *p)
{
	delete p;
}
