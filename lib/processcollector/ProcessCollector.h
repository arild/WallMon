/*
 * Collector.h
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#ifndef PROCESSCOLLECTOR_H_
#define PROCESSCOLLECTOR_H_

#include "Wallmon.h"
#include "ProcessCollector.pb.h"
#include "LinuxProcessMonitor.h"
#include <list>

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
	char *_buffer;
	int _numCores;
	double _totalMemoryMb;
};

#endif /* PROCESSCOLLECTOR_H_ */
