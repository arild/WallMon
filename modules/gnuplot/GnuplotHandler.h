/*
 * ProcessMonitorHandler.h
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#ifndef HANDLER_H_
#define HANDLER_H_

#include "Wallmon.h"
#include "LinuxProcessMonitorLight.h"
#include "gnuplot_i.hpp"
#include "ProcessCollector.h"
#include <vector>
#include <map>
#include "LocalSampler.h"

using namespace std;

// host name -> vector of CPU utilization
typedef ProcessMessage ProcMsg;
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
	LocalSampler *_localSampler;

	void _GenerateCharts();
	void _GenerateCpuChart(vector<vector<double> > &systemCpu, vector<vector<double> > &userCpu);
	void _GenerateMemoryChart(vector<vector<double> > &memory);
	void _GenerateNetworkChart(vector<vector<unsigned int> > &network);
	void _GenerateServerNetworkChart(vector<vector<double > > &networkCollector, vector<double> &networkHandler);
	void _SaveToFile(vector< vector<string> > xy, string firstLineComment);
	int _expectedNumSamplesPerNode;
};

#endif /* HANDLER_H_ */
