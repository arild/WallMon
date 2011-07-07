/*
 * Collector.h
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#ifndef COLLECTOR_H_
#define COLLECTOR_H_

#include "Wallmon.h"

using namespace std;

class TestCollector: public IDataCollectorProtobuf {
public:
	virtual void OnInit(Context *ctx);
	virtual void OnStop();
	virtual void Sample(WallmonMessage *msg);
};

#endif /* COLLECTOR_H_ */
