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
#include "WallView.h"

#define KEY						"PROCESS_ALL_MON"
#define MESSAGE_BUF_SIZE		1024 * 1000
#define SAMPLE_FREQUENCY_MSEC 	1000
#define FRAMES_PER_SEC			2
#define NUM_PROCESSES_TO_DISPLAY	8

unsigned long _FnvHash(string key);
void _ProcessNameToRgbColor(string name, int *r, int *g, int *b);

void Handler::OnInit(Context *ctx)
{
	ctx->key = KEY;
	_boidsApp = NULL;
	_message = new ProcessesMessage();
	_procMap = new ProcMap();

#ifdef ROCKSVV
	WallView w(0, 0, 3, 4);
	if (w.IsWithin()) {
		double x, y, width, height;
		w.GetOrientation(&x, &y, &width, &height);
		_boidsApp = new BoidsApp();
		_boidsApp->SetView(x, y, width, height);
		_boidsApp->Start();
	}
#else
	_boidsApp = new BoidsApp();
//	_boidsApp->SetView(20, 50, 80, 50);
	//_boidsApp->SetView(0, 50, 100, 50);
	_boidsApp->SetView(0, 0, 100, 100);
	_boidsApp->Start();
#endif
}

void Handler::OnStop()
{
	if (_boidsApp != NULL) {
		_boidsApp->Stop();
		delete _boidsApp;
	}

	delete _message;
	delete _procMap;
}

double maxUtil = 0;
void Handler::Handle(WallmonMessage *msg)
{
	if (_boidsApp == NULL)
		return;
	const char *data = msg->data().c_str();
	int length = msg->data().length();
	if (_message->ParseFromArray(data, length) == false)
		LOG(FATAL) << "Protocol buffer parsing failed: ";
	// Go through all process specific messages and read statistics
	for (int i = 0; i < _message->processmessage_size(); i++) {
		ProcessesMessage::ProcessMessage *processMessage = _message->mutable_processmessage(i);

		ProcStat *procStat = NULL;
		string hostname = msg->hostname();
		int pid = processMessage->pid();
		string procMapKey = Handler::_CreateProcMapKey(hostname, pid);

		if (_procMap->count(procMapKey) == 0) {
			procStat = new ProcStat();
			(*_procMap)[procMapKey] = procStat;

			BoidSharedContext *ctx = procStat->boids->cpu;
			_ProcessNameToRgbColor(processMessage->processname(), &ctx->red, &ctx->green,
					&ctx->blue);
			ctx->shape = QUAD;
			_boidsApp->CreateBoid(0, 0, ctx);

			ctx = procStat->boids->memory;
			_ProcessNameToRgbColor(processMessage->processname(), &ctx->red, &ctx->green,
					&ctx->blue);
			ctx->shape = TRIANGLE;
			_boidsApp->CreateBoid(0, 0, ctx);

			ctx = procStat->boids->network;
			_ProcessNameToRgbColor(processMessage->processname(), &ctx->red, &ctx->green,
					&ctx->blue);
			ctx->shape = DIAMOND;
			_boidsApp->CreateBoid(0, 0, ctx);


		} else {
			procStat = (*_procMap)[procMapKey];
		}
		procStat->numSamples += 1;

		// CPU
		double user = processMessage->usercpuutilization();//processMessage->usercpuload();
		double system = processMessage->systemcpuutilization();//processMessage->systemcpuload();
		double totalCpuUtilization = user + system;
		double userCpuRelativeShare = 50;
		if (totalCpuUtilization > 0)
			userCpuRelativeShare = (user / totalCpuUtilization) * (double) 100;
		if (totalCpuUtilization < 0.5)
			procStat->boids->cpu->SetDestination(-1, -1);
		else
			procStat->boids->cpu->SetDestination(totalCpuUtilization, userCpuRelativeShare);

		 // Memory
		procStat->totalMemoryUtilization += (double)processMessage->memoryutilization();
		double avgMemoryUtilization = procStat->totalMemoryUtilization
				/ (double) procStat->numSamples;
		if (processMessage->memoryutilization() < 0.5)
			procStat->boids->memory->SetDestination(-1, -1);
		else
			procStat->boids->memory->SetDestination(processMessage->memoryutilization(),
				avgMemoryUtilization);

		// Network
		double in = processMessage->networkinutilization();
		double out = processMessage->networkoututilization();
		double networkUtilization = in + out;
		if (networkUtilization > maxUtil) {
			maxUtil = networkUtilization;
			LOG(INFO) << "Network util: " << networkUtilization;
		}
		double outNetworkRelativeShare = 50;
		if (networkUtilization > 0)
			outNetworkRelativeShare = (out / networkUtilization) * (double) 100;
		if (networkUtilization < 0.5)
			procStat->boids->network->SetDestination(-1, -1);
		else
			procStat->boids->network->SetDestination(networkUtilization, outNetworkRelativeShare);

		//		LOG(INFO) << "Mem util: " << avgMemoryUtilization;
		//LOG(INFO) << "memory : " << processMessage->memoryutilization();
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

unsigned long _FnvHash(string key)
{
	unsigned char *p = (unsigned char *) key.c_str();
	unsigned long h = 2166136261UL;
	for (int i = 0; i < key.length(); i++)
		h = (h * 16777619) ^ p[i];
	return h;
}

void _ProcessNameToRgbColor(string name, int *r, int *g, int *b)
{
	unsigned long hash = _FnvHash(name);
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
