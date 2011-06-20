/*
 * Collector.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include "System.h"
#include "ProcessCollector.h"
#include <list>

using namespace std;

#define KEY					"gnuplot"
#define SERVER				"129.242.19.58"
#define SAMPLE_FREQUENCY	1000
#define PROCESS_NAME		"wallmond"
#define MESSAGE_BUF_SIZE	4096

extern "C" ProcessCollector *create_collector()
{
	ProcessCollector *p = new ProcessCollector();

	list<string> processNames;
	processNames.push_back(PROCESS_NAME);
	p->SetProcesses(processNames);

	p->context->key = KEY;
	p->context->AddServer(SERVER);
	p->context->sampleFrequencyMsec = SAMPLE_FREQUENCY;

	p->filter->set_processname("");
	p->filter->set_pid(0);
	p->filter->set_usercpuutilization(0);
	p->filter->set_systemcpuutilization(0);
	p->filter->set_memoryutilization(0);

	if (System::HasSupportForProcPidIo()) {
		LOG(INFO) << "support for /proc/<pid>/io detected, monitoring it";
		p->filter->set_networkinutilization(0);
		p->filter->set_networkoututilization(0);
	}
	return p;
}

extern "C" void destroy_collector(ProcessCollector *p)
{
	delete p;
}
