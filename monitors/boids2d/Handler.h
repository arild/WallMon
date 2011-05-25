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
#include "EventHandlerBase.h"

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
	NameTagList *_nameTagList;
	EventHandlerBase *_eventHandler;

	string _CreateProcMapKey(string hostName, int pid);
	unsigned long _OatHash(string key);
	void _ProcessNameToRgbColor(string name, int *r, int *g, int *b);
};

#endif /* HANDLER_H_ */
