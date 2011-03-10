/*
 * ProcessMonitorHandler.h
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#ifndef PROCESSMONITORHANDLER_H_
#define PROCESSMONITORHANDLER_H_

#include <list>
#include <map>
#include <string>
#include <sstream>

#include "Wallmon.h"
#include "Protocol.pb.h"
#include "LinuxProcessMonitor.h"
#include "BarChart.h"

using namespace std;


class ProcStat {
public:
	double userCpuLoad;
	double systemCpuLoad;

	ProcStat()
	{
		userCpuLoad = 0;
		systemCpuLoad = 0;
	}
};
typedef map<string, ProcStat *> HostNamePidMap;

class ProcStatSum {
public:
	HostNamePidMap *hostNamePidMap;
	double totalCpuLoad;

	ProcStatSum()
	{
		hostNamePidMap = hostNamePidMap = new HostNamePidMap();
		totalCpuLoad = 0;
	}

	virtual ~ProcStatSum()
	{
		delete hostNamePidMap;
	}
};
typedef map<string, ProcStatSum *> ProcessNameMap;


class ProcessMonitorHandler: public IDataHandler {
public:
	virtual void OnInit(Context *ctx);
	virtual void OnStop();
	virtual void Handle(void *data, int length);
private:
	void _UpdateBarChart();
	ProcessNameMap *_processNameMap;
	ProcessesMessage *_message;
	vector<double> *_minHeap;
	double _totalCpuLoad;
	BarChart *_barChart;
};

class ProcessMonitorCollector: public IDataCollector {
public:
	virtual void OnInit(Context *ctx);
	virtual void OnStop();
	virtual int Sample(void **data);
private:
	std::list<LinuxProcessMonitor *> *_monitors;
	char *_buffer;
};

#endif /* PROCESSMONITORHANDLER_H_ */
