/*
 * ProcessMonitorHandler.h
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#ifndef HANDLER_H_
#define HANDLER_H_

#include "Wallmon.h"
#include "gnuplot_i.hpp"
#include "ProcessCollector.h"
#include <vector>
#include <map>

using namespace std;

// host name -> vector of CPU utilization
typedef map<string, vector<double> *> DataContainer;

class GnuplotHandler: public IDataHandlerProtobuf {
public:
	virtual void OnInit(Context *ctx);
	virtual void OnStop();
	virtual void Handle(WallmonMessage *msg);

private:
	ProcessCollectorMessage _message;
	Gnuplot _plot;
	vector<double> _x, _yDaemon, _yServer;
	DataContainer _data;
	void _GenerateGraph();
	vector<double> _ComputeAverage();
	void _SaveToFile(vector< vector<string> > xy);
};

#endif /* HANDLER_H_ */
