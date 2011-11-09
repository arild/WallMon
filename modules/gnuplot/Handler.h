/*
 * ProcessMonitorHandler.h
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#ifndef HANDLER_H_
#define HANDLER_H_

#include <vector>
#include <map>
#include <boost/tuple/tuple.hpp>

#include "Wallmon.h"
#include "LinuxProcessMonitorLight.h"
#include "ProcessCollector.h"
#include "LocalSampler.h"
#include "WallView.h"
#include "Stat.h"

using namespace std;

// process name -> host name -> process messages
typedef map<string, vector<ProcessMessage> *> HostToProcsContainer;
typedef map<string, HostToProcsContainer *> ProcNameToHostContainer;

// process name -> Hz -> process messages
typedef map<int, vector<ProcessMessage> * > HzToProcsContainer;
typedef map<string,  HzToProcsContainer *> ProcNameToHzContainer;

// Hz -> wallmon messages
typedef map<uint32_t, vector<WallmonMessage> *> HzToWallmonMessagesContainer;


class GnuplotHandler: public IDataHandlerProtobuf {
public:
	virtual void OnInit(Context *ctx);
	virtual void OnStop();
	virtual void Handle(WallmonMessage *msg);

private:
	Context *_ctx;
	WallView *_wallView;
	ProcessCollectorMessage _msg;
	ProcNameToHostContainer _procNameToHost;
	ProcNameToHzContainer _procNameToHz;
	HzToWallmonMessagesContainer _hzToWallmonMessages;
	Stat<unsigned int> _numServerConnections;
	Stat<double> _networkReceiveMegaBytesPerSec;
	double _previousTimestamp;
	uint64_t _sumBytesNetworkReceive;

	map<string, int> _terminationCond;
	int _numSamples, _expectedTotalNumSamples;
	int _expectedNumSamplesPerNode;
	LocalSampler *_localSampler;
	vector<string> _targetProcesses;

	bool _IsTargetProcess(string processName);
	void _SaveGeneralData();
	void _SaveWallmonSamplingRateData();
	void _MaintainNetworkReceivePerSec(WallmonMessage *msg);
	void _SaveCpuData(vector<Stat<double> > &userCpuUtil, vector<Stat<double> > &systemCpuUtil);
	void _GenerateMemoryChart(vector<vector<double> > &memory);
	void _GenerateNetworkChart(vector<vector<unsigned int> > &network);
	void _GenerateServerNetworkChart(vector<vector<double > > &networkCollector, vector<double> &networkHandler);
	void _SaveToFile(vector< vector<string> > xy, string firstLineComment);
};

#endif /* HANDLER_H_ */
