/*
 * Collector.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include <boost/foreach.hpp>
#include <vector>
#include "ProcessCollector.h"
#include "System.h"
#include "unistd.h"

#define MESSAGE_BUF_SIZE				1024 * 50 // 50KB (350 processes ~ 15 KB)
const double NETWORK_MAX_IN_AND_OUT_BYTES = 1024 * 1024 * 100;

int _numTimesFailedMonitoringProcesses = 0;

ProcessCollector::ProcessCollector(IProcessCollectorController *controller)
{
	context = new Context();
	filter = new ProcessMessage;
	_controller = controller;
}

/**
 * Tells the process collector to only look for defined process names
 * during its life-cycle
 */
void ProcessCollector::SetProcesses(list<string> processNames)
{
	_processNames = processNames;
}

void ProcessCollector::OnInit(Context *ctx)
{
	// The framework injects its own mutable instance of Context, therefore
	// the context instance that the user has been modifying must be copied
	ctx->key = context->key;
	ctx->sampleFrequencyMsec = context->sampleFrequencyMsec;
	ctx->servers = context->servers;
	ctx->includeStatistics = context->includeStatistics;
	delete context;
	context = ctx;

	_numCores = System::GetNumLogicalCores();
	_totalMemoryMb = (double)System::GetTotalMemory() / 1024.;
	// Create a process monitor for each pid on system
	_monitors = new vector<LinuxProcessMonitorLight *>();
	_pidMonitor = new PidMonitor;
	_buffer = new char[MESSAGE_BUF_SIZE];
	memset(_buffer, 0, MESSAGE_BUF_SIZE);

	if (_processNames.size() == 0)
		_FindAllNewProcesses();
	else
		_AddDefinedProcesses();
	_processesMsg.Clear();

	LOG(INFO) << "num cores detected: " << _numCores;
	LOG(INFO) << "total memory detected: " << _totalMemoryMb << " MB";
	LOG(INFO) << "num processes being monitored: " << _monitors->size();
}

void ProcessCollector::OnStop()
{
	delete _buffer;
	delete filter;
}

void ProcessCollector::Sample(WallmonMessage *msg)
{
	// TODO: Investigate why this causes segfault at arbirary intervals
	if (_processNames.size() == 0)
		_FindAllNewProcesses();

	// Drop the BOOST_FOREACH macro due to ~5% overhead. This loop is critical for performance
	for (vector<LinuxProcessMonitorLight *>::iterator it = _monitors->begin(); it != _monitors->end(); it++) {
		LinuxProcessMonitorLight *monitor = (*it);
		monitor->Update();
		ProcessMessage *processMsg = _processesMsg.add_processmessage();

		double util;
		if (filter->has_processname()) {
			processMsg->set_processname(monitor->GetProcessName());
		}
		if (filter->has_pid()) {
			processMsg->set_pid(monitor->pid());
		}
		if (filter->has_usercpuload())
			processMsg->set_usercpuload(monitor->GetUserCpuLoad());
		if (filter->has_systemcpuload())
			processMsg->set_systemcpuload(monitor->GetSystemCpuLoad());
		if (filter->has_usercpuutilization()) {
			util = monitor->GetUserCpuLoad() / (double)(_numCores * 100);
			processMsg->set_usercpuutilization(util * 100.);
		}
		if (filter->has_systemcpuutilization()) {
			util = monitor->GetSystemCpuLoad() / (double)(_numCores * 100);
			processMsg->set_systemcpuutilization(util * 100.);
		}
		if (filter->has_memoryutilization()) {
			util = monitor->GetTotalProgramSize() / (double)_totalMemoryMb;
 			processMsg->set_memoryutilization(util * 100.);
		}
		if (filter->has_networkinbytes()) {
			processMsg->set_networkinbytes(monitor->GetNetworkInInBytes());
		}
		if (filter->has_networkoutbytes()) {
			processMsg->set_networkoutbytes(monitor->GetNetworkOutInBytes());
		}
		if (filter->has_user()) {
			processMsg->set_user(_ps.PidToUser(monitor->pid()));
		}
		if (filter->has_starttime()) {
			processMsg->set_starttime(_ps.PidToStime(monitor->pid()));
		}
		if (filter->has_numthreads()) {
			processMsg->set_numthreads(monitor->numthreads());
		}
	}

	if (_processesMsg.SerializeToArray(_buffer, MESSAGE_BUF_SIZE) != true)
		LOG(ERROR) << "Protocol buffer serialization failed";
	msg->set_data(_buffer, _processesMsg.ByteSize());
	_processesMsg.Clear();

	if (_controller != NULL)
		_controller->AfterSample(context);
}

void ProcessCollector::_FindAllNewProcesses()
{
	_pidMonitor->Update();
	vector<int> pids = _pidMonitor->GetDifference();
	BOOST_FOREACH(int pid, pids)
		_AddProcess(pid);
}

void ProcessCollector::_AddDefinedProcesses()
{
	BOOST_FOREACH(string processName, _processNames)
		_AddProcess(System::GetPid(processName));
}

void ProcessCollector::_AddProcess(int pid)
{
	LinuxProcessMonitorLight *monitor = new LinuxProcessMonitorLight();
	if (monitor->Open(pid) == false) {
		LOG_EVERY_N(INFO, 500) << "failed opening and monitoring processes " << _numTimesFailedMonitoringProcesses++ << " times";
		delete monitor;
		// TODO: Some processes are not possible to monitor (need root?), however pids that are not
		// possible monitor just increases linearly, can not ignore them all
		//_pidMonitor->Ignore(pid);
		return;
	}
	monitor->Update();
	_monitors->push_back(monitor);
}


