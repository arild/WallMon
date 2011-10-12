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
	_pidMonitor = new PidMonitor;
	_buffer = new char[MESSAGE_BUF_SIZE];
	memset(_buffer, 0, MESSAGE_BUF_SIZE);

	if (_processNames.size() == 0)
		_FindAllNewProcesses();
	else
		_AddDefinedProcesses();
	_processesMsg.Clear();
	_numSamples = 0;

	LOG(INFO) << "num cores detected: " << _numCores;
	LOG(INFO) << "total memory detected: " << _totalMemoryMb << " MB";
	LOG(INFO) << "num processes being monitored: " << _monitors.size();
}

void ProcessCollector::OnStop()
{
	delete _buffer;
	delete filter;
}

void ProcessCollector::Sample(WallmonMessage *msg)
{
	// TODO: Investigate why this causes segfault at arbirary intervals
	if (_processNames.size() == 0)// _numSamples++ % 5 == 0)
		_FindAllNewProcesses();

	// Drop the BOOST_FOREACH macro due to ~5% overhead. This loop is critical for performance
	for (int i = 0; i < _monitors.size(); i++) {
		LinuxProcessMonitorLight *monitor = _monitors[i];
		ProcessMessage *processMsg = _processesMsg.add_processmessage();
		if (monitor->Update() == false) {
			// Process is terminated (and not present in procfs)
			_monitors.erase(_monitors.begin() + i);
			processMsg->set_isterminated(true);

			// Continue reading old values stored in the monitor
			// and after that delete it
		}

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
		if (filter->has_user() && monitor->IsFirstTime()) {
			processMsg->set_user(monitor->GetUser());
		}
		if (filter->has_starttime() && monitor->IsFirstTime()) {
			processMsg->set_starttime(monitor->GetStartTime());
		}
		if (filter->has_numthreads()) {
			processMsg->set_numthreads(monitor->numthreads());
		}
		if (filter->has_numpagefaultspersec()) {
			processMsg->set_numpagefaultspersec(monitor->GetTotalNumPageFaultsPerSec());
		}

		if (processMsg->isterminated())
			delete monitor;
	}

	if (_processesMsg.SerializeToArray(_buffer, MESSAGE_BUF_SIZE) != true) {
		LOG(ERROR) << "Protocol buffer serialization failed";
		_processesMsg.Clear();
		return;
	}
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

	bool open = monitor->Open(pid);
	bool update = false;
	if (open)
		update = monitor->Update();

	if (open && update)
		_monitors.push_back(monitor);
	else {
		LOG_EVERY_N(INFO, 500) << "failed opening and monitoring processes " << _numTimesFailedMonitoringProcesses++ << " times";
		delete monitor;
	}
}


