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
typedef ProcessCollectorMessage::ProcessMessage ProcMsg;
typedef map<string, vector<ProcMsg> *> DataContainer;

class GnuplotHandler: public IDataHandlerProtobuf {
public:
	virtual void OnInit(Context *ctx);
	virtual void OnStop();
	virtual void Handle(WallmonMessage *msg);

private:
	ProcessCollectorMessage _message;
	Gnuplot _plot;
	DataContainer _data;
	void _GenerateCpuChart();
	vector<double> _ComputeAverage(vector< vector<double> *> results);
	void _SaveToFile(vector< vector<string> > xy, string firstLineComment);
	int _expectedNumSamplesPerNode;
};

#endif /* HANDLER_H_ */
