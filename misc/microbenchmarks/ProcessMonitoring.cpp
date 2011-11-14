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

ProcessMonitoring::ProcessMonitoring()
{
	BOOST_FOREACH(int pid, System::GetAllPids()) {
		LinuxProcessMonitorLight *monitor = new LinuxProcessMonitorLight();

		bool open = monitor->Open(pid);
		bool update = false;
		if (open)
			update = monitor->Update();

		if (open && update) {
			monitor->SetStatistics(&procfsReadSamples, &procfsParseSamples);
			_monitors.push_back(monitor);
		} else
			delete monitor;

		if (_monitors.size() == 280)
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

	double startProtobufPopulateMsec, startProcfsMsec;
	totalMemoryMb = System::GetTotalMemory();
	numCores = System::GetNumLogicalCores();
	double startSerializationMsec;

	double start = System::GetTimeInMsec();
	ProcessCollectorMessage processesMsg;
	for (int i = 0; i < numSamples; i++) {
		processesMsg.Clear();
		procfsReadSamples.NewSample();
		procfsParseSamples.NewSample();
		protobufPopulateSamples.NewSample();
		protobufSerializationSamples.NewSample();
		totalSample.NewSample();
		double startTotal = System::GetTimeInMsec();

		for (int j = 0; j < _monitors.size(); j++) {
			ProcessMessage *msg = processesMsg.add_processmessage();
			LinuxProcessMonitorLight *m = _monitors[j];
			m->Update();

			startProtobufPopulateMsec = System::GetTimeInMsec();

			msg->set_processname(m->GetProcessName());
			msg->set_pid(m->pid());

			util = m->GetUserCpuLoad() / (double) (numCores * 100);
			msg->set_usercpuutilization(util * 100.);

			util = m->GetSystemCpuLoad() / (double) (numCores * 100);
			msg->set_systemcpuutilization(util * 100.);

			util = m->GetTotalProgramSize() / (double) totalMemoryMb;
			msg->set_memoryutilization(util * 100.);

			msg->set_networkinbytes(m->GetNetworkInBytes());
			msg->set_networkoutbytes(m->GetNetworkOutBytes());

			msg->set_storageinbytes(m->GetStorageInBytes());
			msg->set_storageoutbytes(m->GetStorageOutBytes());

			if (m->IsFirstTime())
				msg->set_user(m->GetUser());

			if (m->IsFirstTime())
				msg->set_starttime(m->GetStartTime());

			msg->set_numthreads(m->numthreads());
			msg->set_numpagefaultspersec(m->GetTotalNumPageFaultsPerSec());

			protobufPopulateSamples.Add(System::GetTimeInMsec() - startProtobufPopulateMsec);
		}

		startSerializationMsec = System::GetTimeInMsec();
		if (processesMsg.SerializeToArray(buf, bufSize) != true)
			cout << "Protocol buffer serialization failed" << endl;
		double ts = System::GetTimeInMsec();
		protobufSerializationSamples.Add(ts - startSerializationMsec);
		totalSample.Add(ts - startTotal);
	}

	double stop = System::GetTimeInMsec();
	double totalTimeMsec = stop - start;

	// Include de-serialization
	int byteSize = processesMsg.ByteSize();
	for (int i = 0; i < numSamples; i++) {
		protobufDeserializationSamples.NewSample();
		start = System::GetTimeInMsec();
		if (processesMsg.ParseFromArray(buf, byteSize) == false)
			cout << "Protocol buffer de-serializing failed" << endl;
		protobufDeserializationSamples.Add(System::GetTimeInMsec() - start);

//		if (processesMsg.SerializeToArray(buf, bufSize) != true)
//			cout << "Protocol buffer serialization failed" << endl;
	}


	cout << endl;

	cout << "total msec    : " << totalTimeMsec << endl;
	cout << "total avg msec: " << totalTimeMsec / (double) numSamples << endl;
	cout << "total avg Hz  : " << 1000 / (double) (totalTimeMsec / (double) numSamples) << endl;

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

	cout << "protobuf populate total msec  : " << protobufPopulateSamples.SumTotal() << endl;
	cout << "protobuf populate avg msec    : " << protobufPopulateSamples.SumAvg() << endl;
	cout << "protobuf populate avg var msec: " << protobufPopulateSamples.VarianceAvg() << endl;
	cout << "protobuf populate avg sd msec : " << protobufPopulateSamples.StandardDeviationAvg() << endl;
	cout << "protobuf populate sum min     : " << protobufPopulateSamples.SumMin() << endl;
	cout << "protobuf populate sum max     : " << protobufPopulateSamples.SumMax() << endl;

	cout << endl;

	cout << "protobuf serialization total msec  : " << protobufSerializationSamples.SumTotal() << endl;
	cout << "protobuf serialization avg msec    : " << protobufSerializationSamples.SumAvg() << endl;
	cout << "protobuf serialization avg var msec: " << protobufSerializationSamples.VarianceAvg() << endl;
	cout << "protobuf serialization avg sd msec : " << protobufSerializationSamples.StandardDeviationAvg() << endl;
	cout << "protobuf serialization sum min     : " << protobufSerializationSamples.SumMin() << endl;
	cout << "protobuf serialization sum max     : " << protobufSerializationSamples.SumMax() << endl;

	cout << endl;


	for (int i = 0; i < totalSample.NumSamples(); i++) {
		double sumOther = totalSample.Sum(i) - (procfsReadSamples.Sum(i) +
					procfsParseSamples.Sum(i) + protobufPopulateSamples.Sum(i) +
					protobufSerializationSamples.Sum(i));
		otherSample.NewSample();
		otherSample.Add(sumOther);
	}

	cout << "other total msec  : " << otherSample.SumTotal() << endl;
	cout << "other avg msec    : " << otherSample.SumAvg() << endl;
	cout << "other avg var msec: " << otherSample.VarianceAvg() << endl;
	cout << "other avg sd msec : " << otherSample.StandardDeviationAvg() << endl;
	cout << "other sum min     : " << otherSample.SumMin() << endl;
	cout << "other sum max     : " << otherSample.SumMax() << endl;

	cout << endl;

	cout << "protobuf de-serialization total msec  : " << protobufDeserializationSamples.SumTotal() << endl;
	cout << "protobuf de-serialization avg msec    : " << protobufDeserializationSamples.SumAvg() << endl;
	cout << "protobuf de-serialization avg var msec: " << protobufDeserializationSamples.VarianceAvg() << endl;
	cout << "protobuf de-serialization avg sd msec : " << protobufDeserializationSamples.StandardDeviationAvg() << endl;
	cout << "protobuf de-serialization sum min     : " << protobufDeserializationSamples.SumMin() << endl;
	cout << "protobuf de-serialization sum max     : " << protobufDeserializationSamples.SumMax() << endl;

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

	for (int j = 0; j < _monitors.size(); j++) {
		LinuxProcessMonitorLight *m = _monitors[j];
		ProcessMessage *msg = procsMsg.add_processmessage();
		m->Update();

		msg->set_processname(m->GetProcessName());
		msg->set_pid(m->pid());

		util = m->GetUserCpuLoad() / (double) (numCores * 100);
		msg->set_usercpuutilization(util * 100.);

		util = m->GetSystemCpuLoad() / (double) (numCores * 100);
		msg->set_systemcpuutilization(util * 100.);

		util = m->GetTotalProgramSize() / (double) totalMemoryMb;
		msg->set_memoryutilization(util * 100.);

		msg->set_networkinbytes(m->GetNetworkInBytes());
		msg->set_networkoutbytes(m->GetNetworkOutBytes());

		msg->set_storageinbytes(m->GetStorageInBytes());
		msg->set_storageoutbytes(m->GetStorageOutBytes());

		if (m->IsFirstTime())
			msg->set_user(m->GetUser());

		if (m->IsFirstTime())
			msg->set_starttime(m->GetStartTime());

		msg->set_numthreads(m->numthreads());
		msg->set_numpagefaultspersec(m->GetTotalNumPageFaultsPerSec());

	}
	cout << "Probuf serialization size benchmark:" << endl;
	cout << "************" << endl;
	cout << "serializing process data for " << _monitors.size() << " processes" << endl;
	if (procsMsg.SerializeToArray(buf, bufSize) != true)
		cout << "Protocol buffer serialization failed" << endl;
	cout << "process data serialization size in bytes: " << procsMsg.ByteSize() << endl;
	cout << "average num bytes per process: " << procsMsg.ByteSize() / _monitors.size() << endl;

}

