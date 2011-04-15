/*
 * Collector.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include <boost/foreach.hpp>

#include "ProcessCollector.h"
#include "System.h"
#include "unistd.h"

#define MESSAGE_BUF_SIZE	(1024 * 1000) * 5

ProcessCollector::ProcessCollector()
{
	context = new Context;
	filter = new ProcessesMessage::ProcessMessage;
}

void ProcessCollector::OnInit(Context *ctx)
{
	ctx->server = context->server;
	ctx->key = context->key;
	ctx->sampleFrequencyMsec = context->sampleFrequencyMsec;
	delete context;
	context = ctx;

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

void ProcessCollector::OnStop()
{
	delete _buffer;
	delete filter;
}

void ProcessCollector::Sample(WallmonMessage *msg)
{
	ProcessesMessage processesMsg;
	for (list<LinuxProcessMonitor *>::iterator it = _monitors->begin(); it != _monitors->end(); it++) {
		LinuxProcessMonitor *monitor = (*it);
		monitor->update();
		ProcessesMessage::ProcessMessage *processMsg = processesMsg.add_processmessage();

		if (filter->has_processname()) {
			string processName = monitor->comm();
			// Remove '(' and ')' from the process name returned from comm()
			processName.erase(processName.length() - 1, 1);
			processName.erase(0, 1);
			processMsg->set_processname(processName);
		}
		if (filter->has_pid()) {
			processMsg->set_pid(monitor->pid());
		}
		if (filter->has_usercpuload())
			processMsg->set_usercpuload(monitor->getUserCPULoad());
		if (filter->has_systemcpuload())
			processMsg->set_systemcpuload(monitor->getSystemCPULoad());
	}

	if (processesMsg.SerializeToArray(_buffer, MESSAGE_BUF_SIZE) != true)
		LOG(ERROR) << "Protocol buffer serialization failed";
	msg->set_data(_buffer, processesMsg.ByteSize());
}
