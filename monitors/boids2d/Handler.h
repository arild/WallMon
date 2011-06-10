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
#include "NameTable.h"
#include "DataContainers.h"

using namespace std;
using namespace boost::tuples;

class Handler: public IDataHandlerProtobuf {
public:
	virtual void OnInit(Context *ctx);
	virtual void OnStop();
	virtual void Handle(WallmonMessage *msg);
private:
	ProcessesMessage *_message;
	WallView *_wallView;
	BoidsApp *_boidsApp;
	Data *_data;
	EventSystemBase *_eventSystem;
	NameTable *_nameTable;
	void _HandleProcessMessage(ProcessesMessage::ProcessMessage &msg, string hostname);
	void _UpdateCommonAggregatedStatistics(ProcessesMessage::ProcessMessage &msg, StatBase &pstat, StatBase &astat);
	void _UpdateProcessStatistics(ProcessesMessage::ProcessMessage &msg, StatBase &pstat);
	void _RankTableItems();
};

#endif /* HANDLER_H_ */
