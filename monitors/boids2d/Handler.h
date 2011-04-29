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
#include "stubs/Wallmon.pb.h"
#include "stubs/ProcessCollector.pb.h"
#include "BoidsApp.h"
#include "BoidSharedContext.h"

using namespace std;


class BoidsContainer {
public:
	BoidSharedContext *cpu;
	BoidSharedContext *memory;
	BoidSharedContext *network;

	BoidsContainer()
	{
		cpu = new BoidSharedContext();
		memory = new BoidSharedContext();
		network = new BoidSharedContext();
	}
};

class ProcStat {
public:
	unsigned int numSamples;
	double avgCpuUtilization;
	double totalMemoryUtilization;
	BoidsContainer *boids;

	ProcStat()
	{
		boids = new BoidsContainer();
		numSamples = 0;
		avgCpuUtilization = 0;
		totalMemoryUtilization = 0;
	}
};
typedef map<string, ProcStat *> ProcMap;

class Handler: public IDataHandlerProtobuf {
public:
	virtual void OnInit(Context *ctx);
	virtual void OnStop();
	virtual void Handle(WallmonMessage *msg);
private:
	ProcessesMessage *_message;
	BoidsApp *_boidsApp;
	ProcMap *_procMap;
	static string _CreateProcMapKey(string hostName, int pid);
};

#endif /* HANDLER_H_ */
