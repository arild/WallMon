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
						_monitors.push_back(monitor);
					}
				}
}

ProcessMonitoring::~ProcessMonitoring()
{
	// TODO Auto-generated destructor stub
}

void ProcessMonitoring::RunProcfsBenchmark(int numSamples, bool verboseProcfs, bool includeProtobuf, bool verboseProtobuf)
{
	double util;
	int totalMemoryMb, numCores, bufSize = 1 << 20;
	char *buf = new char[bufSize];
	double sumProtobufPackMsec = 0;
	double sumProtobufSerializationMsec = 0;
	double sumProcfsMsec = 0;
	double startProtobufPackMsec, startProcfsMsec;
	totalMemoryMb = System::GetTotalMemory();
	numCores = System::GetNumLogicalCores();
	double startSerializationMsec;

	if (includeProtobuf)
		cout << endl << "Procfs benchmark WITH protobuf:" << endl;
	else
		cout << endl << "Procfs benchmark WITHOUT protobuf:" << endl;
	cout << "************" << endl;

	double start = System::GetTimeInMsec();

	ProcessCollectorMessage procsMsg;
	for (int i = 0; i < numSamples; i++) {
		procsMsg.Clear();
		for (vector<LinuxProcessMonitorLight *>::iterator it = _monitors.begin(); it
				!= _monitors.end(); it++) {
			LinuxProcessMonitorLight *m = (*it);
			if (verboseProcfs)
				startProcfsMsec = System::GetTimeInMsec();
			m->Update();
			if (verboseProcfs)
				sumProcfsMsec += System::GetTimeInMsec() - startProcfsMsec;

			if (includeProtobuf) {
				if (verboseProtobuf)
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

				if (verboseProtobuf)
					sumProtobufPackMsec += System::GetTimeInMsec() - startProtobufPackMsec;
			}
		}

		if (includeProtobuf) {
			if (verboseProtobuf)
				startSerializationMsec = System::GetTimeInMsec();
			if (procsMsg.SerializeToArray(buf, bufSize) != true)
				cout << "Protocol buffer serialization failed" << endl;
			if (verboseProtobuf)
				sumProtobufSerializationMsec += System::GetTimeInMsec() - startSerializationMsec;
		}
	}

	double stop = System::GetTimeInMsec();
	double totalTimeMsec = stop - start;
	cout << "total time msec: " << totalTimeMsec << endl;

	double totalProcfs = totalTimeMsec;
	if (verboseProcfs) {
		totalProcfs = sumProcfsMsec;
		cout << "total procfs msec: " << sumProcfsMsec << endl;
	}
	double hz = numSamples / (double)(totalProcfs / (double)1000);
	cout << "average procfs msec: " << 1000/(double)hz << " (" << hz << " Hz)" << endl;


	if (verboseProtobuf) {
		cout << "total protobuf pack msec: " << sumProtobufPackMsec << endl;
		cout << "average protobuf pack msec: " << sumProtobufPackMsec / (double)numSamples << endl;
		cout << "total serialization msec: " << sumProtobufSerializationMsec << endl;
		cout << "average protobuf serialization msec: " << sumProtobufSerializationMsec / (double)numSamples << endl;
	}

	if (verboseProtobuf && verboseProtobuf) {
		double totalOtherMsec = totalTimeMsec - sumProcfsMsec - sumProtobufPackMsec - sumProtobufSerializationMsec;
		double avgOtherMsec = totalOtherMsec / (double)numSamples;
		cout << "total other msec: " << totalOtherMsec << endl;
		cout << "average other msec: " << avgOtherMsec << endl;
	}

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

