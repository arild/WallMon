/*
 * ProcessMonitorHandler.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include "ProcessMonitor.h"
#include "System.h"
#include "unistd.h"

#define KEY		"PROCESS_MON"

void Handler::OnInit(Context *ctx)
{
	ctx->key = KEY;
	_dataPacket = new DataPacket();
#ifdef GRAPHICS
	LOG(INFO) << "Setting up graphics";
	_barChart = new BarChart();
#endif
}

void Handler::OnStop()
{
	delete _dataPacket;
}

void Handler::Handle(void *data, int length)
{
	if (_dataPacket->ParseFromArray(data, length) == false)
		;//LOG(ERROR) << "Protocol buffer parsing failed: ";
	double cpuload = _dataPacket->usercpuload();
	cpuload += _dataPacket->systemcpuload();
#ifdef GRAPHICS
	_barChart->Render(_dataPacket->hostname(), (int)cpuload);
#endif
}

void Collector::OnInit(Context *ctx)
{
	ctx->server = "129.242.19.57";
	ctx->key = KEY;
	ctx->sampleFrequencyMsec = 100;

	int pid = System::GetPid("opera");
	if (pid == -1)
		pid = System::GetPid("wallmond");
	if (pid == -1)
		LOG(ERROR) << "process name not found";

	_monitor = new LinuxProcessMonitor(pid);
	_dataPacket = new DataPacket();
	_buffer = new char[1024];
	memset(_buffer, 0, 1024);
}

void Collector::OnStop()
{
	delete _monitor;
	delete _dataPacket;
	delete _buffer;
}

int Collector::Sample(void **data)
{
	_monitor->update();

	int numThreads = _monitor->num_threads();
	double userTime = _monitor->getUserTime();
	double systemTime = _monitor->getSystemTime();
	double userCpuLoad = _monitor->getUserCPULoad();
	double systemCpuLoad = _monitor->getSystemCPULoad();
	char hostname[100];
	gethostname(hostname, 100);

	_dataPacket->set_numthreads(numThreads);
	_dataPacket->set_usertime(userTime);
	_dataPacket->set_systemtime(systemTime);
	_dataPacket->set_usercpuload(userCpuLoad);
	_dataPacket->set_systemcpuload(systemCpuLoad);
	_dataPacket->set_hostname(hostname);

	if (_dataPacket->SerializeToArray(_buffer, 1024) != true)
		LOG(ERROR) << "Protocol buffer serialization failed";
	*data = _buffer;
	return _dataPacket->ByteSize();
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

extern "C" Collector *create_collector()
{
	return new Collector;
}

extern "C" void destroy_collector(Collector *p)
{
	delete p;
}
