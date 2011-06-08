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
#include "Wallmon.pb.h"
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
	double userCpuLoad;
	double systemCpuLoad;

	ProcNameStat()
	{
		procMap = new ProcMap();
		userCpuLoad = 0;
		systemCpuLoad = 0;
	}

	virtual ~ProcNameStat()
	{
		delete procMap;
	}

	bool Has(string procKey)
	{
		if (procMap->count(procKey) == 0)
			return false;
		return true;
	}

	ProcStat *Get(string procKey)
	{
		if (procMap->count(procKey) == 0)
			// Unique process not present
			(*procMap)[procKey] = new ProcStat();
		return (*procMap)[procKey];
	}

	static string CreateProcMapKey(string hostName, int pid)
	{
		stringstream ss;
		ss << hostName << pid;
		return ss.str();
	}
};
typedef map<string, ProcNameStat *> ProcNameMap;


class Handler: public IDataHandlerProtobuf {
public:
	virtual void OnInit(Context *ctx);
	virtual void OnStop();
	virtual void Handle(WallmonMessage *msg);
private:
	void _UpdateBarChart();
	ProcNameMap *_processNameMap;
	ProcessesMessage *_message;
	vector<double> *_minHeap;
	BarChart *_barChart;
	double _timestamp;
};

#endif /* HANDLER_H_ */
