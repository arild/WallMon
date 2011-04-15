/*
 * Handler.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include <boost/foreach.hpp>
#include <algorithm>
#include <vector>
#include "Handler.h"
#include "System.h"
#include "unistd.h"

#define KEY						"PROCESS_ALL_MON"
#define MESSAGE_BUF_SIZE		1024 * 1000
#define SAMPLE_FREQUENCY_MSEC 	1000
#define FRAMES_PER_SEC			2
#define NUM_PROCESSES_TO_DISPLAY	8

unsigned long fnv_hash(string key)
{
	unsigned char *p = (unsigned char *)key.c_str();
	unsigned long h = 2166136261UL;
	for (int i = 0; i < key.length(); i++)
		h = ( h * 16777619 ) ^ p[i];
	return h;
}

void processNameToRgbColor(string name, int *r, int *g, int *b)
{
	unsigned long hash = fnv_hash(name);
	*r = hash & 0xFF;
	*g = (hash >> 8) & 0xFF;
	*b = (hash >> 16) & 0xFF;
	if (*r < 20)
		*r = 20;
	if (*g < 20)
		*g = 20;
	if (*b < 20)
		*b = 20;
}

void Handler::OnInit(Context *ctx)
{
	ctx->key = KEY;
#ifdef GRAPHICS
	_boidsApp = new BoidsApp();
	_boidsApp->Start();
#endif
	_message = new ProcessesMessage();
	_procMap = new ProcMap();
}

void Handler::OnStop()
{
#ifdef GRAPHICS
	_boidsApp->Stop();
	delete _boidsApp;
#endif
	delete _message;
	delete _procMap;
}

void Handler::Handle(WallmonMessage *msg)
{
	const char *data = msg->data().c_str();
	int length = msg->data().length();
	if (_message->ParseFromArray(data, length) == false)
		LOG(FATAL) << "Protocol buffer parsing failed: ";
	// Go through all process specific messages and read statistics
	for (int i = 0; i < _message->processmessage_size(); i++) {
		ProcessesMessage::ProcessMessage *processMessage = _message->mutable_processmessage(i);
		double user = processMessage->usercpuload();
		double system = processMessage->systemcpuload();
		double totalCpuLoad = user + system;
		double userCpuRelativeShare = 50;
		if (totalCpuLoad > 0)
			userCpuRelativeShare = (user / totalCpuLoad) * (float)100;

		ProcStat *procStat = NULL;
		string hostName = msg->hostname();
		int pid = processMessage->pid();
		string procMapKey = Handler::_CreateProcMapKey(hostName, pid);

		if (_procMap->count(procMapKey) == 0) {
			BoidSharedContext *ctx = new BoidSharedContext();
			processNameToRgbColor(processMessage->processname(), &ctx->red, &ctx->green, &ctx->blue);

			ctx->SetDestination(totalCpuLoad, userCpuRelativeShare);
			_boidsApp->CreateBoid(0, 0, ctx);

			procStat = new ProcStat();
			procStat->boidSharedCtx = ctx;
			(*_procMap)[procMapKey] = procStat;
		}
		else {
			procStat = (*_procMap)[procMapKey];
			procStat->boidSharedCtx->SetDestination(totalCpuLoad, userCpuRelativeShare);
		}
//		LOG(INFO) << "user  : " << processMessage->usercpuload();
//		LOG(INFO) << "system: " << processMessage->systemcpuload();
	}
}

string Handler::_CreateProcMapKey(string hostName, int pid)
{
	stringstream ss;
	ss << hostName << pid;
	return ss.str();
}

// class factories - needed to bootstrap object orientation with dlfcn.h
extern "C" Handler *create_handler()
{
	return new Handler;
}

extern "C" void destroy_handler(Handler *p)
{
	delete p;
}
