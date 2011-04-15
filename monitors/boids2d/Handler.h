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
#include "Wallmon.pb.h"
#include "processcollector/ProcessCollector.pb.h"
#include "BoidsApp.h"
#include "BoidSharedContext.h"

using namespace std;

class ProcStat {
public:
	string processName;
	string hostName;
	int pid;
	double userCpuLoad;
	double systemCpuLoad;
	BoidSharedContext *boidSharedCtx;

	ProcStat()
	{
		userCpuLoad = 0;
		systemCpuLoad = 0;
		boidSharedCtx = NULL;
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
