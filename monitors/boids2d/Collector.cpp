
#include <glog/logging.h>
#include "ProcessCollector.h"
#include "System.h"
#include "WallView.h"

#define KEY		"PROCESS_ALL_MON"
#define SAMPLE_FREQUENCY_MSEC 	1000

extern "C" ProcessCollector *create_collector()
{
	ProcessCollector *p = new ProcessCollector();
	p->context->key = KEY;
#ifdef ROCKSVV
	vector<string> s = WallView(0, 0, 3, 4).Get();
	p->context->AddServers(s);
#else
	p->context->AddServer("129.242.19.57");
#endif
	p->context->sampleFrequencyMsec = SAMPLE_FREQUENCY_MSEC;
	p->filter->set_processname("");
	p->filter->set_pid(0);
	//p->filter->set_usercpuload(0);
	//p->filter->set_systemcpuload(0);
	p->filter->set_usercpuutilization(0);
	p->filter->set_systemcpuutilization(0);
	p->filter->set_memoryutilization(0);

	if (System::HasSupportForProcPidIo()) {
		p->filter->set_networkinutilization(0);
		p->filter->set_networkoututilization(0);
	}
	return p;
}

extern "C" void destroy_collector(ProcessCollector *p)
{
	delete p;
}
