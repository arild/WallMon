#include <glog/logging.h>
#include "ProcessCollector.h"
#include "System.h"
#include "WallView.h"
#include "PortForwarder.h"

#define SAMPLE_FREQUENCY_MSEC 	2000

extern "C" ProcessCollector *create_collector()
{
	ProcessCollector *p = new ProcessCollector();
	p->context->key = "CHART";
	p->context->includeStatistics = true;
	// Forward to desktop
	p->context->AddServer("arild.dyndns-work.com");
	//p->context->AddServers(PortForwarder::HostnamesToRocksvvRootNodeMapping(servers));
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
