
#include <glog/logging.h>
#include "processcollector/ProcessCollector.h"

#define KEY		"PROCESS_ALL_MON"
#define SAMPLE_FREQUENCY_MSEC 	1000

extern "C" ProcessCollector *create_collector()
{
	ProcessCollector *p = new ProcessCollector();
	p->context->key = KEY;
	p->context->server = "129.242.19.57";
	p->context->sampleFrequencyMsec = SAMPLE_FREQUENCY_MSEC;
	p->filter->set_processname("");
	p->filter->set_pid(0);
	p->filter->set_usercpuload(0);
	p->filter->set_systemcpuload(0);
	return p;
}

extern "C" void destroy_collector(ProcessCollector *p)
{
	delete p;
}
