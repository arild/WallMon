
#include <glog/logging.h>
#include "ProcessCollector.h"
#include "System.h"
#include "WallView.h"
#include "PortForwarder.h"

#define KEY		"BOIDS"
#define SAMPLE_FREQUENCY_MSEC 	1000

extern "C" ProcessCollector *create_collector()
{
	ProcessCollector *p = new ProcessCollector();
	p->context->key = KEY;
	p->context->includeStatistics = true;
	if (System::IsRocksvvCluster()) {
		vector<string> servers = WallView(2,2,2,2).GetGrid();
//		vector<string> servers = WallView(0,0,7,4).GetGrid();
		p->context->AddServers(servers);
	}
	else if (System::GetHostname().compare(0, 5, "arild") == 0) {
		p->context->AddServer("localhost");
	}
	else {
		// Forward to desktop
		p->context->AddServer("arild.dyndns-work.com");
		//p->context->AddServers(PortForwarder::HostnamesToRocksvvRootNodeMapping(servers));
	}
	p->context->sampleFrequencyMsec = SAMPLE_FREQUENCY_MSEC;
	p->filter->set_processname("");
	p->filter->set_pid(0);
	p->filter->set_usercpuutilization(0);
	p->filter->set_systemcpuutilization(0);
	p->filter->set_memoryutilization(0);
//	p->filter->set_networkinutilization(0);
//	p->filter->set_networkoututilization(0);
	p->filter->set_networkinbytes(0);
	p->filter->set_networkoutbytes(0);
	p->filter->set_user("");
	p->filter->set_starttime("");
	p->filter->set_numthreads(0);
	return p;
}

extern "C" void destroy_collector(ProcessCollector *p)
{
	delete p;
}
