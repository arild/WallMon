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
#include "LinuxProcessMonitor.h"
#include "PidMonitor.h"

using namespace std;


class ProcessCollector: public IDataCollectorProtobuf {
public:
	ProcessesMessage::ProcessMessage *filter;
	Context *context;
	ProcessCollector();

	// Wallmon API
	virtual void OnInit(Context *ctx);
	virtual void OnStop();
	virtual void Sample(WallmonMessage *msg);
private:
	vector<LinuxProcessMonitor *> *_monitors;
	PidMonitor *_pidMonitor;
	char *_buffer;
	int _numCores;
	double _totalMemoryMb;
	bool _hasSupportForProcIo;
	void _AddProcesses();
	void _AddProcess(int pid);
};

#endif /* PROCESSCOLLECTOR_H_ */
