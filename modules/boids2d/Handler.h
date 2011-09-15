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
#include <boost/tuple/tuple.hpp>

#include "Wallmon.h"
#include "stubs/Wallmon.pb.h"
#include "stubs/ProcessCollector.pb.h"
#include "BoidsApp.h"
#include "BoidSharedContext.h"
#include "EventSystemBase.h"
#include "Table.h"
#include "DataContainers.h"

using namespace std;
using namespace boost::tuples;

class Handler: public IDataHandlerProtobuf {
public:
	virtual void OnInit(Context *ctx);
	virtual void OnStop();
	virtual void Handle(WallmonMessage *msg);
private:
	ProcessCollectorMessage *_message;
	WallView *_wallView;
	BoidsApp *_boidsApp;
	Data *_data;
	EventSystemBase *_eventSystem;
	Table *_table;
	void _HandleProcessMessage(ProcessMessage &msg);
	void _UpdateCommonAggregatedStatistics(ProcessMessage &msg, StatBase &pstat, StatBase &astat);
	void _UpdateProcessStatistics(ProcessMessage &msg, StatBase &pstat);
	float _LinearToLogarithmicAxisValue(float linearValue);
};

#endif /* HANDLER_H_ */
