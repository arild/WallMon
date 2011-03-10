/*
 * ProcessMonitorHandler.h
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#ifndef HANDLER_H_
#define HANDLER_H_

#include <list>
#include <map>
#include <string>
#include <sstream>

#include "Wallmon.h"
#include "Protocol.pb.h"
#include "BarChart.h"

using namespace std;

/**
 * Statistics about a single unique process
 */
class ProcStat {
public:
	string processName;
	string hostName;
	int pid;
	double userCpuLoad;
	double systemCpuLoad;

	ProcStat()
	{
		userCpuLoad = 0;
		systemCpuLoad = 0;
	}
};


/**
 * Contains aggregated statistics about processes with the same name,
 * and has references to each process involved in the calculation.
 */
typedef map<string, ProcStat *> ProcMap;
class ProcNameStat {
public:
	ProcMap *procMap;
	double totalUserCpuLoad;
	double totalSystemCpuLoad;

	ProcNameStat()
	{
		procMap = new ProcMap();
		totalUserCpuLoad = 0;
		totalSystemCpuLoad = 0;
	}

	virtual ~ProcNameStat()
	{
		delete procMap;
	}

	ProcStat *Get(string key)
	{
		if (procMap->count(key) == 0)
			// Host name not registered
			(*procMap)[key] = new ProcStat();
		return (*procMap)[key];
	}

	static string CreateProcMapKey(string hostName, int pid)
	{
		stringstream ss;
		ss << hostName << pid;
		return ss.str();
	}
};
typedef map<string, ProcNameStat *> ProcNameMap;


class Handler: public IDataHandler {
public:
	virtual void OnInit(Context *ctx);
	virtual void OnStop();
	virtual void Handle(void *data, int length);
private:
	void _UpdateBarChart();
	ProcNameMap *_processNameMap;
	ProcessesMessage *_message;
	vector<double> *_minHeap;
	BarChart *_barChart;
	double _timestamp;
};

#endif /* HANDLER_H_ */
