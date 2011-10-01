/*
 * ProcessMonitoring.cpp
 *
 *  Created on: Jul 9, 2011
 *      Author: arild
 */
#include <boost/foreach.hpp>
#include <iostream>
#include "System.h"
#include "ProcessMonitoring.h"
#include "stubs/ProcessCollector.pb.h"

typedef ProcessCollectorMessage::ProcessMessage ProcMsg;
//typedef ProcessMessage ProcMsg;

ProcessMonitoring::ProcessMonitoring()
{
	BOOST_FOREACH(int pid, *System::GetAllPids())
	{
		LinuxProcessMonitorLight *monitor = new LinuxProcessMonitorLight();
		if (monitor->Open(pid) == false) {
			cout << "failed opening pid = " << pid << endl;
			delete monitor;
		} else {
			monitor->Update();
			monitor->SetStatistics(&procfsReadSamples, &procfsParseSamples);
			_monitors.push_back(monitor);
		}
		if (_monitors.size() == 318)
			break;
	}
	cout << "monitoring " << _monitors.size() << " processes" << endl;
}

ProcessMonitoring::~ProcessMonitoring()
{
	// TODO Auto-generated destructor stub
}

void ProcessMonitoring::RunProcfsBenchmark(int numSamples)
{
	double util;
	int totalMemoryMb, numCores, bufSize = 1 << 20;
	char *buf = new char[bufSize];

	double startProtobufPackMsec, startProcfsMsec;
	totalMemoryMb = System::GetTotalMemory();
	numCores = System::GetNumLogicalCores();
	double startSerializationMsec;

	double start = System::GetTimeInMsec();
	ProcessCollectorMessage procsMsg;
	for (int i = 0; i < numSamples; i++) {

		procfsReadSamples.NewSample();
		procfsParseSamples.NewSample();
		protobufPackSamples.NewSample();
		protobufSerializationSamples.NewSample();

		procsMsg.Clear();
		for (int j = 0; j < _monitors.size(); j++) {
			LinuxProcessMonitorLight *m = _monitors[j];
			m->Update();

			startProtobufPackMsec = System::GetTimeInMsec();
			ProcMsg *procMsg = procsMsg.add_processmessage();

			procMsg->set_processname(m->GetProcessName());
			procMsg->set_pid(m->pid());

			util = m->GetUserCpuLoad() / (double) (numCores * 100);
			procMsg->set_usercpuutilization(util * 100.);

			util = m->GetSystemCpuLoad() / (double) (numCores * 100);
			procMsg->set_systemcpuutilization(util * 100.);

			util = m->GetTotalProgramSize() / (double) totalMemoryMb;
			procMsg->set_memoryutilization(util * 100.);

			procMsg->set_networkinbytes(m->GetNetworkInInBytes());
			procMsg->set_networkoutbytes(m->GetNetworkOutInBytes());

			protobufPackSamples.Add(System::GetTimeInMsec() - startProtobufPackMsec);
		}

		startSerializationMsec = System::GetTimeInMsec();
		if (procsMsg.SerializeToArray(buf, bufSize) != true)
			cout << "Protocol buffer serialization failed" << endl;
		protobufSerializationSamples.Add(System::GetTimeInMsec() - startSerializationMsec);
	}

	double stop = System::GetTimeInMsec();
	double totalTimeMsec = stop - start;

	cout << endl;

	cout << "total msec    : " << totalTimeMsec << endl;
	cout << "total avg msec: " << totalTimeMsec/(double)numSamples << endl;
	cout << "total avg Hz  : " << 1000/(double)(totalTimeMsec/(double)numSamples) << endl;

	cout << endl;

	cout << "procfs read total msec  : " << procfsReadSamples.SumTotal() << endl;
	cout << "procfs read avg msec    : " << procfsReadSamples.SumAvg() << endl;
	cout << "procfs read avg var msec: " << procfsReadSamples.VarianceAvg() << endl;
	cout << "procfs read avg sd msec : " << procfsReadSamples.StandardDeviationAvg() << endl;
	cout << "procfs read sum min     : " << procfsReadSamples.SumMin() << endl;
	cout << "procfs read sum max     : " << procfsReadSamples.SumMax() << endl;

	cout << endl;

	cout << "procfs parse total msec  : " << procfsParseSamples.SumTotal() << endl;
	cout << "procfs parse avg msec    : " << procfsParseSamples.SumAvg() << endl;
	cout << "procfs parse avg var msec: " << procfsParseSamples.VarianceAvg() << endl;
	cout << "procfs parse avg sd msec : " << procfsParseSamples.StandardDeviationAvg() << endl;
	cout << "procfs parse sum min     : " << procfsParseSamples.SumMin() << endl;
	cout << "procfs parse sum max     : " << procfsParseSamples.SumMax() << endl;

	cout << endl;

	cout << "protobuf pack total msec  : " << protobufPackSamples.SumTotal() << endl;
	cout << "protobuf pack avg msec    : " << protobufPackSamples.SumAvg() << endl;
	cout << "protobuf pack avg var msec: " << protobufPackSamples.VarianceAvg() << endl;
	cout << "protobuf pack avg sd msec : " << protobufPackSamples.StandardDeviationAvg() << endl;
	cout << "protobuf pack min         : " << protobufPackSamples.SumMin() << endl;
	cout << "protobuf pack max         : " << protobufPackSamples.SumMax() << endl;

	cout << endl;

	cout << "protobuf serialization total msec  : " << protobufSerializationSamples.SumTotal() << endl;
	cout << "protobuf serialization avg msec    : " << protobufSerializationSamples.SumAvg() << endl;
	cout << "protobuf serialization avg var msec: " << protobufSerializationSamples.VarianceAvg() << endl;
	cout << "protobuf serialization avg sd msec : " << protobufSerializationSamples.StandardDeviationAvg() << endl;
	cout << "protobuf serialization sum min     : " << protobufSerializationSamples.SumMin() << endl;
	cout << "protobuf serialization sum max     : " << protobufSerializationSamples.SumMax() << endl;

	delete buf;
}

void ProcessMonitoring::RunProtobufBenchmark()
{
	ProcessCollectorMessage procsMsg;
	double util;
	int totalMemoryMb = System::GetTotalMemory();
	int numCores = System::GetNumLogicalCores();
	int bufSize = 1024 * 50;
	char buf[bufSize];

	for (vector<LinuxProcessMonitorLight *>::iterator it = _monitors.begin(); it != _monitors.end(); it++) {
		LinuxProcessMonitorLight *m = (*it);
		m->Update();
		ProcMsg *procMsg = procsMsg.add_processmessage();

		procMsg->set_processname(m->GetProcessName());

		util = m->GetUserCpuLoad() / (double) (numCores * 100);
		procMsg->set_usercpuutilization(util * 100.);

		util = m->GetSystemCpuLoad() / (double) (numCores * 100);
		procMsg->set_systemcpuutilization(util * 100.);

		util = m->GetTotalProgramSize() / (double) totalMemoryMb;
		procMsg->set_memoryutilization(util * 100.);

		procMsg->set_networkinbytes(m->GetNetworkInInBytes());
		procMsg->set_networkoutbytes(m->GetNetworkOutInBytes());
	}

	cout << endl << "Probuf serialization size benchmark:" << endl;
	cout << "************" << endl;
	cout << "serializing process data for " << _monitors.size() << " processes" << endl;
	if (procsMsg.SerializeToArray(buf, bufSize) != true)
		cout << "Protocol buffer serialization failed" << endl;
	cout << "process data serialization size in bytes: " << procsMsg.ByteSize() << endl;
	cout << "average num bytes per process: " << procsMsg.ByteSize() / _monitors.size() << endl;

}

