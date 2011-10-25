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
#include <cmath>
#include "Handler.h"
#include "System.h"
#include "unistd.h"
#include "WallView.h"
#include "SdlMouseEventFetcher.h"
#include "ShoutEventSystem.h"

#define KEY							"BOIDS"
#define MESSAGE_BUF_SIZE			1024 * 1000
double FILTER_THRESHOLD = 1.0;
double NETWORK_MAX_IN_AND_OUT_BYTES = 1024 * 1024 * 50;
int numUniqueProcesses = 0;

void Handler::OnInit(Context *ctx)
{
	ctx->key = KEY;
	_message = new ProcessCollectorMessage();
	_data = new Data();

#ifdef ROCKSVV
	/**
	 * On the display wall cluster it is likely that only a a sub-set of available
	 * tiles should be used. In such a situation, the display area of each tile must be adjusted
	 */
	_wallView = new WallView(2,1,3,2);
	//	_wallView = new WallView(0,0,7,4);
	if (_wallView->IsTileWithin() == false)
	return;
	System::AttachToLocalDisplay(); // Does not seem to be required on rocksvv cluster
	double x, y, width, height;
	_eventSystem = new ShoutEventSystem();
	_wallView->GetDisplayArea(&x, &y, &width, &height);
	_boidsApp = new BoidsApp(TILE_SCREEN_WIDTH, TILE_SCREEN_HEIGHT, _eventSystem, _wallView);
	_boidsApp->SetDisplayArea(x, y, width, height);
#else
	/**
	 * In a local environment (only one computer/screen) the event system is told that
	 * the 'entire' display wall is used, and coordinates are mapped thereafter. Also,
	 * the single screen is told to show everything. Visually, this works fine since the
	 * hard-coded resolution (1600x768) is a down-scaled version of the display wall resolution.
	 */
	//	_wallView = new WallView(3, 0, 3, 4);
	//	_eventSystem = new ShoutEventSystem();
	_wallView = new WallView(0, 0, WALL_WIDHT, WALL_HEIGHT, "tile-0-0");
	_eventSystem = new SdlMouseEventFetcher();
	_boidsApp = new BoidsApp(1600, 768, _eventSystem, _wallView);

	_boidsApp->SetDisplayArea(0, 0, WALL_SCREEN_WIDTH, WALL_SCREEN_HEIGHT);
#endif
	// Common operations
	_eventSystem->SetWallView(_wallView);
	_table = _boidsApp->GetNameTable();
	_eventSystem->Start();
	LOG(INFO) << "starting BoidsApp...";
	_boidsApp->Start();
#ifdef ROCKSVV
	System::BringWallmonServerWindowToFront();
#endif
	LOG(INFO) << "BoidsApp started";
	VisualBase::boidsApp = _boidsApp;
	VisualBase::table = _table;
}

void Handler::OnStop()
{
	_eventSystem->Stop();
	if (_boidsApp != NULL) {
		// Triggered only by nodes that was within the defined grid (on the display wall)
		_boidsApp->Stop();
		delete _boidsApp;
	}
	delete _message;
	delete _data;
	delete _wallView;
}

void Handler::Handle(WallmonMessage *msg)
{
	if (_boidsApp == NULL)
		return;
	const char *data = msg->data().c_str();
	int length = msg->data().length();
	if (_message->ParseFromArray(data, length) == false)
		LOG(FATAL) << "Protocol buffer parsing failed: ";
	// Go through all process specific messages
	for (int i = 0; i < _message->processmessage_size(); i++) {
		ProcessMessage *processMessage = _message->mutable_processmessage(i);
		processMessage->set_hostname(msg->hostname());
		_HandleProcessMessage(*processMessage);
	}
}

/**
 * Updates statistics in data indexes and boids
 */
void Handler::_HandleProcessMessage(ProcessMessage &msg)
{
	Proc *proc = _data->Update(msg);
	if (proc == NULL)
		// Resources related to process deleted
		return;

	_UpdateProcessStatistics(msg, *proc->stat);

	// CPU
	double user = proc->stat->userCpuUtilization;
	double system = proc->stat->systemCpuUtilization;
	double totalCpuUtilization = user + system;
	double userCpuRelativeShare = 50;
	if (totalCpuUtilization > 0)
		userCpuRelativeShare = (user / totalCpuUtilization) * (double) 100;
	if (BoidSharedContext::useLogarithmicAxis)
		totalCpuUtilization = _LinearToLogarithmicAxisValue(totalCpuUtilization);
	if (totalCpuUtilization < FILTER_THRESHOLD)
		proc->visual->cpu->ctx->SetDestination(-1, -1);
	else
		proc->visual->cpu->ctx->SetDestination(totalCpuUtilization, userCpuRelativeShare);

	// Memory
	double avgMemoryUtilization = proc->stat->memoryUtilizationSum
			/ (double) proc->stat->numSamples;
	double memUtil = proc->stat->memoryUtilization;
	if (BoidSharedContext::useLogarithmicAxis)
		memUtil = _LinearToLogarithmicAxisValue(memUtil);
	if (proc->stat->memoryUtilization < FILTER_THRESHOLD)
		proc->visual->memory->ctx->SetDestination(-1, -1);
	else
		proc->visual->memory->ctx->SetDestination(memUtil, msg.numpagefaultspersec() * 0.1);

	// Network
	double in = proc->stat->networkInUtilization;
	double out = proc->stat->networkOutUtilization;
	double networkUtilization = in + out;
	double networkRelativeShare = 50;
	if (networkUtilization > 0)
		networkRelativeShare = (out / networkUtilization) * (double) 100;
	if (BoidSharedContext::useLogarithmicAxis)
		networkUtilization = _LinearToLogarithmicAxisValue(networkUtilization);
	if (networkUtilization < FILTER_THRESHOLD)
		proc->visual->network->ctx->SetDestination(-1, -1);
	else
		proc->visual->network->ctx->SetDestination(networkUtilization, networkRelativeShare);

	// Storage
	in = proc->stat->storageInUtilization;
	out = proc->stat->storageOutUtilization;
	double storageUtilization = in + out;
	double inStorageRelativeShare = 50;
	if (storageUtilization > 0)
		inStorageRelativeShare = (out / storageUtilization) * (double) 100;
	if (BoidSharedContext::useLogarithmicAxis)
		storageUtilization = _LinearToLogarithmicAxisValue(storageUtilization);
	if (storageUtilization < FILTER_THRESHOLD)
		proc->visual->storage->ctx->SetDestination(-1, -1);
	else
		proc->visual->storage->ctx->SetDestination(storageUtilization, inStorageRelativeShare);

	// Table visual
	TableItem *item = proc->visual->tableItem;
	if (msg.has_processname())
		item->SetProcName(msg.processname());
	if (msg.has_hostname())
		item->SetHostName(msg.hostname());
	if (msg.has_user())
		item->SetUser(msg.user());
	if (msg.has_starttime())
		item->SetStartTime(msg.starttime());
	if (msg.has_numthreads()) {
		stringstream ss;
		ss << msg.numthreads();
		item->SetNumThreads(ss.str());
	}
	if (msg.has_pid()) {
		stringstream ss;
		ss << msg.pid();
		item->SetPid(ss.str());
	}

	// Ranking of processes. Computation of score takes all metrics into account
	float newScore = (proc->stat->userCpuUtilization + proc->stat->systemCpuUtilization)
			+ (proc->stat->networkInUtilization + proc->stat->networkOutUtilization) * 1.5
			+ (proc->stat->storageInUtilization + proc->stat->storageOutUtilization) * 1.5
			+ proc->stat->memoryUtilization * 0.5;
	proc->visual->tableItem->SetScore(newScore);
}

void Handler::_UpdateProcessStatistics(ProcessMessage &msg, StatBase &pstat)
{
	pstat.userCpuUtilization = msg.usercpuutilization();
	pstat.systemCpuUtilization = msg.systemcpuutilization();
	pstat.memoryUtilization = msg.memoryutilization();

	pstat.networkInUtilization = (msg.networkinbytes() / (double) NETWORK_MAX_IN_AND_OUT_BYTES)
			* (double) 100;
	pstat.networkOutUtilization = (msg.networkoutbytes() / (double) NETWORK_MAX_IN_AND_OUT_BYTES)
			* (double) 100;

	pstat.storageInUtilization = (msg.storageinbytes() / (double) NETWORK_MAX_IN_AND_OUT_BYTES)
			* (double) 100;
	pstat.storageOutUtilization = (msg.storageoutbytes() / (double) NETWORK_MAX_IN_AND_OUT_BYTES)
			* (double) 100;

	pstat.numSamples += 1;
}

float Handler::_LinearToLogarithmicAxisValue(float linearValue)
{
	return (log2(linearValue) + 1) * 12.5;
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

