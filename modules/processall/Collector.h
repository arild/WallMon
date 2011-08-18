/*
 * Collector.h
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#ifndef COLLECTOR_H_
#define COLLECTOR_H_

#include "Wallmon.h"
#include "Protocol.pb.h"
#include "LinuxProcessMonitor.h"
#include <list>

using namespace std;

class Collector: public IDataCollectorProtobuf {
public:
	virtual void OnInit(Context *ctx);
	virtual void OnStop();
	virtual void Sample(WallmonMessage *msg);
private:
	list<LinuxProcessMonitor *> *_monitors;
	char *_buffer;
	Context *_ctx;
};

#endif /* COLLECTOR_H_ */
