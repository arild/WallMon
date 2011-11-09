/*
 * Handler.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include <boost/foreach.hpp>
#include <stdio.h>
#include <sstream>
#include <set>
#include "Handler.h"
#include "Common.h"
#include "System.h"
#include <boost/assign/std/vector.hpp> // for 'operator+=()'
using namespace boost::assign; // bring 'operator+=()' into scope

//double minScheduleDurationMsec;
//double maxScheduleDurationMsec;
double totalScheduleDurationMsec;
double totalScheduleDriftMsec;

double minSampleDurationMsec;
double maxSampleDurationMsec;
double totalSampleDurationMsec;

double minMemory;
double maxMemory;
double totalMemory;

unsigned int minNetBytes;
unsigned int maxNetBytes;
unsigned int totalNetBytes;
double avgNetBytes;

unsigned int sumProcessMessageBytes;

int NUM_NODES = 0;


void GnuplotHandler::OnInit(Context *ctx)
{
	_wallView = new WallView(0,0,7,4);
//	_wallView = new WallView(2,1,3,2);
//	if (!_wallView->IsTileWithin() && System::GetHostname().compare(0, 5, "arild") != 0) {
//		LOG(INFO) << "terminating handler and server";
//		exit(0);
//	}


	ctx->key = "gnuplot";
	ctx->includeStatistics = true;
	_expectedNumSamplesPerNode = get_total_num_samples();
	_numSamples = 0;
	_expectedTotalNumSamples = 9999;
	_targetProcesses += "wallmons", "wallmond";
	_ctx = ctx;

	_numServerConnections.NewSample();

	_previousTimestamp = -1;
	_sumBytesNetworkReceive = 0;
}

void GnuplotHandler::OnStop()
{
}

void GnuplotHandler::Handle(WallmonMessage *msg)
{
	const char *data = msg->data().c_str();
	int length = msg->data().length();

	if (_msg.ParseFromArray(data, length) == false)
		LOG(FATAL) << "Protocol buffer parsing failed: ";

	if (msg->has_scheduledriftmsec())
		totalScheduleDriftMsec += msg->scheduledriftmsec();
	if (msg->has_scheduledurationmsec())
		totalScheduleDurationMsec += msg->scheduledurationmsec();

	uint32_t hz = (uint32_t)(1000 / msg->samplefrequencymsec());

	// hostname -> num samples
	map<string, int>::iterator it = _terminationCond.find(msg->hostname());
	int numSamples = 1;
	if (it == _terminationCond.end()) {
		// new host
		numSamples = _terminationCond[msg->hostname()] + 1;
		_terminationCond[msg->hostname()] = numSamples;
		_expectedTotalNumSamples = _expectedNumSamplesPerNode * _terminationCond.size();
	}
	else
		_terminationCond[msg->hostname()] = 1;

	if (numSamples >= _expectedNumSamplesPerNode) {
		LOG(INFO) << "too many samples (" << numSamples << ") from: " << msg->hostname() << ", discarding sample";
		return;
	}
	_numSamples++;

	_MaintainNetworkReceivePerSec(msg);
	if (msg->has_daemonnumserverconnections())
		_numServerConnections.Add(msg->daemonnumserverconnections());

	// Hz -> queue sizes
	HzToWallmonMessagesContainer::iterator it6 = _hzToWallmonMessages.find(hz);
	vector<WallmonMessage> *wallmonMessages;
	if (it6 == _hzToWallmonMessages.end()) {
		wallmonMessages = new vector<WallmonMessage>;
		_hzToWallmonMessages.insert(make_pair(hz, wallmonMessages));
	} else {
		wallmonMessages = it6->second;
	}
	wallmonMessages->push_back(*msg);

	// Go through all process messages
	bool isFirstTime = true;
	for (int i = 0; i < _msg.processmessage_size(); i++) {
		ProcessMessage *procMsg = _msg.mutable_processmessage(i);
		if (!_IsTargetProcess(procMsg->processname()))
			// Skip process, not interesting
			continue;

		/* GENERAL DATA */

		// process name -> hostname
		ProcNameToHostContainer::iterator it2 = _procNameToHost.find(procMsg->processname());
		HostToProcsContainer *hostToProcs;
		if (it2 == _procNameToHost.end()) {
			hostToProcs = new map<string, vector<ProcessMessage> *>;
			_procNameToHost[procMsg->processname()] = hostToProcs;
		} else {
			hostToProcs = it2->second;
		}

		// hostname -> process messages
		HostToProcsContainer::iterator it3 = hostToProcs->find(msg->hostname());
		vector<ProcessMessage> *procs;
		if (it3 == hostToProcs->end()) {
			procs = new vector<ProcessMessage>;
			hostToProcs->insert(make_pair(msg->hostname(), procs));
		} else {
			procs = it3->second;
		}
		procs->push_back(*procMsg);


		/* WALLMON SPECIFIC DATA */

		// process name -> hz
		ProcNameToHzContainer::iterator it4 = _procNameToHz.find(procMsg->processname());
		HzToProcsContainer *hzToProcs;
		if (it4 == _procNameToHz.end()) {
			hzToProcs = new HzToProcsContainer;
			_procNameToHz[procMsg->processname()] = hzToProcs;
		} else {
			hzToProcs = it4->second;
		}

		// Hz -> process messages
		HzToProcsContainer::iterator it5 = hzToProcs->find(hz);
		if (it5 == hzToProcs->end()) {
			procs = new vector<ProcessMessage>;
			hzToProcs->insert(make_pair(hz, procs));
		} else {
			procs = it5->second;
		}
		procs->push_back(*procMsg);

		LOG_EVERY_N(INFO, 500) << "Sampling status: " << _numSamples << "/" << _expectedNumSamplesPerNode << " | " << msg->hostname();
		if (_numSamples == _expectedTotalNumSamples) {
			LOG(INFO) << "all samples received (" << _expectedTotalNumSamples << "), saving data and generating charts...";

			if (_wallView->IsLowerLeft() || System::GetHostname().compare(0, 5, "arild") == 0) {
	//			_SaveGeneralData();
				_SaveWallmonSamplingRateData();
				LOG(INFO) << "charts generated, exiting";

			}
			exit(0);
		}
	}

}

bool GnuplotHandler::_IsTargetProcess(string processName)
{
	BOOST_FOREACH(string s, _targetProcesses)
		if (processName.compare(s) == 0)
			return true;
	return false;
}

void GnuplotHandler::_SaveGeneralData()
{
	// Each Stat instance represents all samples for a given metric for a given process,
	// meaning that each sample consists of sub-samples from the different nodes.
	// Each sample within a Stat instance represents all samples for a given node
	vector<Stat<double> > cpuUserUtilVector, cpuSystemUtilVector;
	vector<Stat<uint64_t> > memoryBytesVector;
	vector<Stat<uint32_t> > networkInBytesVector, networkOutBytesVector;

	// Go trough all process names
	BOOST_FOREACH (ProcNameToHostContainer::value_type &item, _procNameToHost) {
		Stat<double> cpuUserUtil(item.first), cpuSystemUtil(item.first);
		Stat<uint64_t> memoryBytes(item.first);
		Stat<uint32_t> networkInBytes(item.first), networkOutBytes(item.first);

		// Go through all hosts where process name is available
		BOOST_FOREACH (HostToProcsContainer::value_type &item2, *item.second) {
			cpuUserUtil.NewSample();
			cpuSystemUtil.NewSample();
			memoryBytes.NewSample();
			networkInBytes.NewSample();
			networkOutBytes.NewSample();

			LOG(INFO) << "traversing host: " << item2.first;

			// Go through all process messages within given host
			BOOST_FOREACH (ProcessMessage msg, *item2.second) {
				cpuUserUtil.Add(msg.usercpuutilization());
				cpuSystemUtil.Add(msg.systemcpuutilization());
				memoryBytes.Add(msg.memorybytes());
				networkInBytes.Add(msg.networkinbytes());
				networkOutBytes.Add(msg.networkoutbytes());
			}

			cpuUserUtilVector.push_back(cpuUserUtil);
			cpuSystemUtilVector.push_back(cpuSystemUtil);
			memoryBytesVector.push_back(memoryBytes);
			networkInBytesVector.push_back(networkInBytes);
			networkOutBytesVector.push_back(networkOutBytes);
		}
	}

	StorageFlags flags;
	flags.avg = true;
	flags.stdDev = true;
	flags.min = true;
	flags.max = true;

//	flags.filename = cpuUserUtilVector[0].GetPrefix() + "_cpu_user_util.dat";
//	cpuUserUtilVector[0].Save(cpuUserUtilVector, flags);
//
//	flags.filename = cpuSystemUtilVector[0].GetPrefix() + "_cpu_system_util.dat";
//	cpuSystemUtilVector[0].Save(cpuSystemUtilVector, flags);
//
//	flags.filename = memoryBytesVector[0].GetPrefix() + "_memory_bytes.dat";
//	memoryBytesVector[0].Save(memoryBytesVector, flags);
//
//	flags.filename = networkInBytesVector[0].GetPrefix() + "_network_receive_bytes.dat";
//	networkInBytesVector[0].Save(networkInBytesVector, flags);
//
//	flags.filename = networkOutBytesVector[0].GetPrefix() + "_network_send_bytes.dat";
//	networkOutBytesVector[0].Save(networkOutBytesVector, flags);
}

void GnuplotHandler::_SaveWallmonSamplingRateData()
{
	// Each Stat instance represents all samples for a given metric for a given process,
	// meaning that each sample consists of sub-samples from the different nodes.
	// Each sample within a Stat instance represents all samples for a given node
	vector<Stat<double> > cpuUserUtilVector, cpuSystemUtilVector, memoryMbVector;
	vector<int> hzVector;
	bool isFirstTime = true;

	// Go trough all process names
	BOOST_FOREACH (ProcNameToHzContainer::value_type &item, _procNameToHz) {
		LOG(INFO) << "PROC NAME: " << item.first;
		Stat<double> cpuUserUtil(item.first), cpuSystemUtil(item.first),
				memoryMb(item.first), networkInMb(item.first), networkOutMb(item.first);

		// Go through Hz
		int hzIndex = 0;
		BOOST_FOREACH (HzToProcsContainer::value_type &item2, *item.second) {
			cpuUserUtil.NewSample();
			cpuSystemUtil.NewSample();
			memoryMb.NewSample();
			if (isFirstTime) {
				hzVector.push_back(item2.first);
			}

			// Go through all process messages within given host
			BOOST_FOREACH (ProcessMessage msg, *item2.second) {
				cpuUserUtil.Add(msg.usercpuutilization());
				cpuSystemUtil.Add(msg.systemcpuutilization());
				memoryMb.Add(msg.memorybytes() / (double)(1024*1024));
			}
		}
		isFirstTime = false;

		cpuUserUtilVector.push_back(cpuUserUtil);
		cpuSystemUtilVector.push_back(cpuSystemUtil);
		memoryMbVector.push_back(memoryMb);
	}

	StorageFlags flags;
	flags.avg = true;
	flags.variance = true;
	flags.stdDev = true;
	flags.min = true;
	flags.max = true;
	flags.generateLinearStartColumn = true;
	flags.startColumn = &hzVector;

	for (int i = 0; i < cpuUserUtilVector.size(); i++) {
		flags.filename = cpuUserUtilVector[i].GetPrefix() + "_cpu_user_util.dat";
		cpuUserUtilVector[i].Save(flags);

		flags.filename = cpuSystemUtilVector[i].GetPrefix() + "_cpu_system_util.dat";
		cpuSystemUtilVector[i].Save(flags);

		flags.filename = memoryMbVector[i].GetPrefix() + "_memory_megabytes.dat";
		memoryMbVector[i].Save(flags);
	}

	// Generate statistics from WallmonMessages
	Stat<uint32_t> serverQueueSize, daemonQueueSize, actualHz;
	Stat<double> wallmonPacketLatencyMsec, sampleDurationMsec;

	// Go trough Hz
	BOOST_FOREACH (HzToWallmonMessagesContainer::value_type &item, _hzToWallmonMessages) {
		serverQueueSize.NewSample();
		daemonQueueSize.NewSample();
		wallmonPacketLatencyMsec.NewSample();
		sampleDurationMsec.NewSample();
		actualHz.NewSample();

		BOOST_FOREACH(WallmonMessage msg, *item.second) {
			serverQueueSize.Add(msg.serverqueuesize());
			daemonQueueSize.Add(msg.daemonqueuesize());
			wallmonPacketLatencyMsec.Add(msg.servertimestampmsec() - msg.daemontimestampmsec());
			sampleDurationMsec.Add(msg.sampledurationmsec());
			if (msg.has_actualhz())
				actualHz.Add(msg.actualhz());
		}

	}
	flags.filename = "server_queue_size.dat";
	serverQueueSize.Save(flags);

	flags.filename = "daemon_queue_size.dat";
	daemonQueueSize.Save(flags);

	flags.filename = "wallmon_packet_latency_msec.dat";
	wallmonPacketLatencyMsec.Save(flags);

	flags.filename = "wallmond_sample_duration_msec.dat";
	sampleDurationMsec.Save(flags);

	flags.filename = "wallmond_actual_hz.dat";
	actualHz.Save(flags);

	flags.startColumn = NULL;
	flags.generateLinearStartColumn = true;
	flags.filename = "wallmons_network_receive_megabytes.dat";
	_networkReceiveMegaBytesPerSec.Save(flags);

	flags.filename = "num_server_connections.dat";
	flags.generateLinearStartColumn = false;
	_numServerConnections.Save(flags);
}

void GnuplotHandler::_MaintainNetworkReceivePerSec(WallmonMessage *msg)
{
	if (_previousTimestamp < 0) {
		_previousTimestamp = msg->servertimestampmsec();
		_sumBytesNetworkReceive = msg->networkmessagesizebytes();
		return;
	}

	if (msg->servertimestampmsec() - _previousTimestamp >= 1000) {
		_previousTimestamp = msg->servertimestampmsec();
		_networkReceiveMegaBytesPerSec.NewSample();
		_networkReceiveMegaBytesPerSec.Add(_sumBytesNetworkReceive / (double)(1024*1024));
		_sumBytesNetworkReceive = 0;
	}
	_sumBytesNetworkReceive += msg->networkmessagesizebytes();
}

void GnuplotHandler::_SaveCpuData(vector<Stat<double> > &userCpuUtil, vector<Stat<double> > &systemCpuUtil)
{

}

void GnuplotHandler::_GenerateMemoryChart(vector<vector<double> > & memory)
{
//	_plot.reset_all();
//	_plot << "set terminal postscript enhanced color";
//	_plot << "set grid noxtics nomxtics ytics nomytics noztics nomztics nox2tics nomx2tics noy2tics nomy2tics nocbtics nomcbtics";
//	_plot << "set output \"memory_chart.ps\"";
//	_plot << "set boxwidth 0.75 absolute";
//	_plot << "set style fill solid 1.00 border -1";
//	_plot << "set style data histogram";
//	_plot << "set ylabel \"Average Memory Utilization in Percent\"";
//	_plot << "set xlabel \"Hz (Number of samples per second)\"";
//	_plot << "plot \"tmp.dat\" usi 2:xtic(1) w boxes title \"\"";
//	System::RunCommand((string)"ps2pdf memory_chart.ps");
//	System::RunCommand((string)"rm memory_chart.ps");
//	LOG(INFO) << "memory chart generated";
}

void GnuplotHandler::_GenerateNetworkChart(vector<vector<unsigned int> > & network)
{
//	_plot.reset_all();
//	_plot << "set terminal postscript enhanced color";
//	_plot << "set grid noxtics nomxtics ytics nomytics noztics nomztics nox2tics nomx2tics noy2tics nomy2tics nocbtics nomcbtics";
//	_plot << "set output \"network_chart.ps\"";
//	_plot << "set boxwidth 0.75 absolute";
//	_plot << "set style fill solid 1.00 border -1";
//	_plot << "set style data histogram";
//	_plot << "set ylabel \"Average Network Consumption in KB\"";
//	_plot << "set xlabel \"Hz (Number of samples per second)\"";
//	_plot << "plot \"tmp.dat\" usi 2:xtic(1) w boxes title \"\"";
//	System::RunCommand((string)"ps2pdf network_chart.ps");
//	System::RunCommand((string)"rm network_chart.ps");
//	LOG(INFO) << "network chart generated";

}

void GnuplotHandler::_GenerateServerNetworkChart(vector<vector<double > > &networkCollector, vector<double> &networkHandler)
{
	//vector<string> y2 = _ToString<double>(_ComputeSum<double>(networkCollector));
//	vector<double> expectedMb;
//
//	avgNetBytes *= 5;
//	for (int i = 1; i < NUM_SAMPLE_INTERVALS; i++) {
//		double val = (get_frequency_in_hz(i) * avgNetBytes) / (double)(1<<20);
//		expectedMb.push_back(val);
//	}
//	vector<string> x = _ToString<int>(get_sample_frequencies_in_hz_exclude_warmup());
//	vector<string> y1 = _ToString<double>(networkHandler);
//	vector<string> y2 =  _ToString<double>(expectedMb);
//
//	if (y1.size() != x.size()) {
//		LOG(WARNING) << "num elements in handler sampled data do not match: " << y1.size() << " | " << x.size();
//	}
//
//
//	vector< vector<string> > xy;
//	xy.push_back(x);
//	xy.push_back(y1);
//	xy.push_back(y2);
//	_SaveToFile(xy, "Hz actual_usage ideal_usage");

//	_plot.reset_all();
//	_plot << "set terminal postscript enhanced color";
//	_plot << "set grid noxtics nomxtics ytics nomytics noztics nomztics nox2tics nomx2tics noy2tics nomy2tics nocbtics nomcbtics";
//	_plot << "set key left top";
//	_plot << "set output \"server_network_chart.ps\"";
//	_plot << "set boxwidth 0.75 absolute";
//	_plot << "set style fill solid 1.00 border";
//	_plot << "set style data histogram";
//	_plot << "set style histogram cluster gap 1";
//	_plot << "set ylabel \"Bandwidth (Mbytes/s)\"";
//	_plot << "set xlabel \"Hz (Number of samples per second)\"";
//	_plot << "plot \"tmp.dat\" using 2:xtic(1) t \"actual bandwidth\", '' using 3 t \"expected bandwidth\"";
//
//	System::RunCommand((string)"ps2pdf server_network_chart.ps");
//	System::RunCommand((string)"rm server_network_chart.ps");
//	LOG(INFO) << "network chart generated";
}

// class factories - needed to bootstrap object orientation with dlfcn.h
extern "C" GnuplotHandler *create_handler()
{
	return new GnuplotHandler;
}

extern "C" void destroy_handler(GnuplotHandler *p)
{
	delete p;
}












