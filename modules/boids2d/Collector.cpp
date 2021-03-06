
#include <glog/logging.h>
#include "ProcessCollector.h"
#include "System.h"
#include "WallView.h"
#include "PortForwarder.h"

#define KEY		"BOIDS"
#define SAMPLE_FREQUENCY_MSEC 	750

extern "C" ProcessCollector *create_collector()
{
	ProcessCollector *p = new ProcessCollector();
	p->context->key = KEY;
	p->context->includeStatistics = true;
	vector<string> servers = WallView(2,1,3,2).GetGrid();
	if (System::IsRocksvvCluster()) {
		p->context->AddServers(servers);
		//		if (System::IsRocksvvClusterRootNode())
//			p->context->AddServers(PortForwarder::HostnamesToRocksvvRootNodeMapping(servers));
//		else
//			p->context->AddServers(servers);
	}
	else if (System::IsIceCluster())
		p->context->AddServers(PortForwarder::HostnamesToRocksvvRootNodeMapping(servers));
	else
		// Forward to desktop
		p->context->AddServer("localhost");

	p->context->sampleFrequencyMsec = SAMPLE_FREQUENCY_MSEC;
	p->filter->set_processname("");
	p->filter->set_pid(0);
	p->filter->set_usercpuutilization(0);
	p->filter->set_systemcpuutilization(0);
	p->filter->set_memoryutilization(0);
	p->filter->set_networkinbytes(0);
	p->filter->set_networkoutbytes(0);
	p->filter->set_storageinbytes(0);
	p->filter->set_storageoutbytes(0);
	p->filter->set_numreadsystemcallspersec(0);
	p->filter->set_numwritesystemcallspersec(0);
	p->filter->set_user("");
	p->filter->set_starttime("");
	p->filter->set_numthreads(0);
	p->filter->set_numpagefaultspersec(0);
	return p;
}

extern "C" void destroy_collector(ProcessCollector *p)
{
	delete p;
}
