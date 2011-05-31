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
#include "SdlMouseEventFetcher.h"

#define KEY							"BOIDS"
#define MESSAGE_BUF_SIZE			1024 * 1000
#define SAMPLE_FREQUENCY_MSEC 		1000
#define FRAMES_PER_SEC				2
#define NUM_PROCESSES_TO_DISPLAY	8


void Handler::OnInit(Context *ctx)
{
	ctx->key = KEY;
	_boidsApp = NULL;
	_message = new ProcessesMessage();
	_procMap = new ProcMap();

	_eventHandler = new EventHandlerBase();
	Queue<TouchEventQueueItem> *touchEventQueue = _eventHandler->GetOutputQueue();
	_eventHandler->Start();

#ifdef ROCKSVV
	WallView w(2, 1, 3, 3);
	if (w.IsTileWithin()) {
		double x, y, width, height;
		w.GetDisplayArea(&x, &y, &width, &height);
		_boidsApp = new BoidsApp(TILE_SCREEN_WIDTH, TILE_SCREEN_HEIGHT, touchEventQueue);
		_boidsApp->SetDisplayArea(x, y, width, height);
		_nameTagList = _boidsApp->CreateNameTagList();
		_boidsApp->Start();
	}
#else
	_boidsApp = new BoidsApp(1600, 768, touchEventQueue);
	_nameTagList = _boidsApp->CreateNameTagList();
	_boidsApp->Start();
	_boidsApp->SetDisplayArea(0, 0, WALL_SCREEN_WIDTH, WALL_SCREEN_HEIGHT);
#endif
}

void Handler::OnStop()
{
	_eventHandler->Stop();
	if (_boidsApp != NULL) {
		_boidsApp->Stop();
		delete _boidsApp;
	}
	delete _message;
	delete _procMap;
}

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
		string procMapKey = _CreateProcMapKey(hostname, pid);

		if (_procMap->count(procMapKey) == 0) {
			procStat = new ProcStat();
			(*_procMap)[procMapKey] = procStat;

			BoidSharedContext *ctx = procStat->boids->cpu;
			_ProcessNameToRgbColor(processMessage->processname(), &ctx->red, &ctx->green,
					&ctx->blue);
			ctx->boidShape = QUAD;
			_boidsApp->CreateBoid(ctx);

			ctx = procStat->boids->memory;
			_ProcessNameToRgbColor(processMessage->processname(), &ctx->red, &ctx->green,
					&ctx->blue);
			ctx->boidShape = TRIANGLE;
			_boidsApp->CreateBoid(ctx);

			ctx = procStat->boids->network;
			_ProcessNameToRgbColor(processMessage->processname(), &ctx->red, &ctx->green,
					&ctx->blue);
			ctx->boidShape = DIAMOND;
			_boidsApp->CreateBoid(ctx);
		} else {
			procStat = (*_procMap)[procMapKey];
		}
		procStat->numSamples += 1;

		double utilFilterFactor = 0.0;

		// CPU
		double user = processMessage->usercpuutilization();//processMessage->usercpuload();
		double system = processMessage->systemcpuutilization();//processMessage->systemcpuload();
		double totalCpuUtilization = user + system;
		double userCpuRelativeShare = 50;
		if (totalCpuUtilization > 0)
			userCpuRelativeShare = (user / totalCpuUtilization) * (double) 100;
		if (totalCpuUtilization < utilFilterFactor)
			procStat->boids->cpu->SetDestination(-1, -1);
		else {
			BoidSharedContext *b = procStat->boids->cpu;
			b->SetDestination(totalCpuUtilization, userCpuRelativeShare);
			NameTag tag(processMessage->processname(), b->red, b->green, b->blue);
			_nameTagList->Add(tag);
		}

		// Memory
		procStat->totalMemoryUtilization += (double) processMessage->memoryutilization();
		double avgMemoryUtilization = procStat->totalMemoryUtilization
				/ (double) procStat->numSamples;
		if (processMessage->memoryutilization() < utilFilterFactor)
			procStat->boids->memory->SetDestination(-1, -1);
		else
			procStat->boids->memory->SetDestination(processMessage->memoryutilization(),
					avgMemoryUtilization);

		// Network
		double in = processMessage->networkinutilization();
		double out = processMessage->networkoututilization();
		double networkUtilization = in + out;
		double outNetworkRelativeShare = 50;
		if (networkUtilization > 0)
			outNetworkRelativeShare = (out / networkUtilization) * (double) 100;
		if (networkUtilization < utilFilterFactor)
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

void Handler::_ProcessNameToRgbColor(string name, int *r, int *g, int *b)
{
	unsigned long hash = _OatHash(name);
	*r = hash & 0xFF;
	*g = (hash >> 8) & 0xFF;
	*b = (hash >> 16) & 0xFF;
	// Make sure color does not become too dark
	if (*r < 20)
		*r = 20;
	if (*g < 20)
		*g = 20;
	if (*b < 20)
		*b = 20;
}

/**
 * One-at-a-time hash by Bob Jenkins: http://en.wikipedia.org/wiki/Jenkins_hash_function
 *
 * This hash function appears to have sufficient avalanch effect, especially compared
 * to the hash function found in boost (<boost/functional/hash.hpp>).
 */
unsigned long Handler::_OatHash(string key)
{
	unsigned char *p = (unsigned char *)key.c_str();
	unsigned long h = 0;
	int i;

	for (i = 0; i < key.length(); i++) {
		h += p[i];
		h += (h << 10);
		h ^= (h >> 6);
	}

	h += (h << 3);
	h ^= (h >> 11);
	h += (h << 15);

	return h;
}

/**
 * class factories - needed to bootstrap object orientation with dlfcn.h
 */
extern "C" Handler *create_handler()
{
	return new Handler;
}

extern "C" void destroy_handler(Handler *p)
{
	delete p;
}
