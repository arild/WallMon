/*
 * Collector.h
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#ifndef PROCESSCOLLECTOR_H_
#define PROCESSCOLLECTOR_H_

#include "Wallmon.h"
#include <list>
#include "stubs/ProcessCollector.pb.h"
#include "LinuxProcessMonitorLight.h"
#include "PidMonitor.h"
#include "Ps.h"

using namespace std;


class IProcessCollectorController {
public:
	virtual ~IProcessCollectorController() {}
	virtual void AfterSample(Context *ctx) = 0;
};

class ProcessCollector: public IDataCollectorProtobuf {
public:
	ProcessMessage *filter;
	Context *context;
	ProcessCollector(IProcessCollectorController *controller = NULL);
	void SetProcesses(list<string> processNames);

	// Wallmon API
	virtual void OnInit(Context *ctx);
	virtual void OnStop();
	virtual void Sample(WallmonMessage *msg);
private:
	vector<LinuxProcessMonitorLight *> *_monitors;
	PidMonitor *_pidMonitor;
	Ps _ps;
	list<string> _processNames;
	char *_buffer;
	int _numCores;
	double _totalMemoryMb;
	ProcessCollectorMessage _processesMsg;
	IProcessCollectorController *_controller;
	void _FindAllNewProcesses();
	void _AddDefinedProcesses();
	void _AddProcess(int pid);
};

#endif /* PROCESSCOLLECTOR_H_ */
