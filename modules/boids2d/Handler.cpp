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
#include "ShoutEventSystem.h"

#define KEY							"BOIDS"
#define MESSAGE_BUF_SIZE			1024 * 1000
#define FILTER_THRESHOLD 			2.0

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
	_wallView = new WallView(2, 0, 3, 4);
	if (_wallView->IsTileWithin() == false)
		return;
	double x, y, width, height;
	_eventSystem = new ShoutEventSystem();
	_wallView->GetDisplayArea(&x, &y, &width, &height);
	_boidsApp = new BoidsApp(TILE_SCREEN_WIDTH, TILE_SCREEN_HEIGHT, _eventSystem);
	_boidsApp->SetDisplayArea(x, y, width, height);
#else
	/**
	 * In a local environment (only one computer/screen) the event system is told that
	 * the 'entire' display wall is used, and coordinates are mapped thereafter. Also,
	 * the single screen is told to show everything.
	 */
//	_wallView = new WallView(3, 0, 3, 4);
//	_eventSystem = new ShoutEventSystem();
	_wallView = new WallView(0, 0, WALL_WIDHT, WALL_HEIGHT);
	_eventSystem = new SdlMouseEventFetcher();
	_boidsApp = new BoidsApp(1600, 768, _eventSystem);


	_boidsApp->SetDisplayArea(0, 0, WALL_SCREEN_WIDTH, WALL_SCREEN_HEIGHT);
#endif
	// Common operations
	_eventSystem->SetWallView(_wallView);
	_table = _boidsApp->GetNameTable();
	_eventSystem->Start();
	LOG(INFO) << "starting BoidsApp...";
	_boidsApp->Start();
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
	_RankTableItems();
}

/**
 * Updates statistics in data indexes and boids
 */
void Handler::_HandleProcessMessage(ProcessMessage &msg)
{
	DataUpdate update = _data->Update(msg);
	Proc *proc = update.proc;
	ProcName *procName = update.procName;
	Node *node = update.node;

	//_UpdateCommonAggregatedStatistics(msg, *proc->stat, *procName->stat);
	_UpdateProcessStatistics(msg, *proc->stat);
	// CPU
	double user = proc->stat->userCpuUtilization;
	double system = proc->stat->systemCpuUtilization;
	double totalCpuUtilization = user + system;
	double userCpuRelativeShare = 50;
	if (totalCpuUtilization > 0)
		userCpuRelativeShare = (user / totalCpuUtilization) * (double) 100;
	if (totalCpuUtilization < FILTER_THRESHOLD)
		proc->visual->cpu->SetDestination(-1, -1);
	else
		proc->visual->cpu->SetDestination(totalCpuUtilization, userCpuRelativeShare);

	// Memory
	double avgMemoryUtilization = proc->stat->memoryUtilizationSum
			/ (double) proc->stat->numSamples;
	if (proc->stat->memoryUtilization < FILTER_THRESHOLD)
		proc->visual->memory->SetDestination(-1, -1);
	else
		proc->visual->memory->SetDestination(proc->stat->memoryUtilization, avgMemoryUtilization);

	// Network
	double in = proc->stat->networkInUtilization;
	double out = proc->stat->networkOutUtilization;
	double networkUtilization = in + out;
	double outNetworkRelativeShare = 50;
	if (networkUtilization > 0)
		outNetworkRelativeShare = (out / networkUtilization) * (double) 100;
	if (networkUtilization < FILTER_THRESHOLD)
		proc->visual->network->SetDestination(-1, -1);
	else
		proc->visual->network->SetDestination(networkUtilization, outNetworkRelativeShare);

	proc->visual->tableItem->score += (totalCpuUtilization * 0.1);
}

/**
 * Boiler plate code for maintaining aggregated values
 *
 * @param msg  A process specific message with fresh data
 * @param pstat  The old values for the same process (in the message)
 * @param astat  The aggregated statistics for some metric
 */
void Handler::_UpdateCommonAggregatedStatistics(ProcessMessage &msg,
		StatBase &pstat, StatBase &astat)
{
	// CPU
	// Subtract old values
	astat.userCpuUtilization -= pstat.userCpuUtilization;
	astat.systemCpuUtilization -= pstat.systemCpuUtilization;
	// Add updated values
	astat.userCpuUtilization += msg.usercpuutilization();
	astat.systemCpuUtilization += msg.systemcpuutilization();

	// Memory
	astat.memoryUtilization -= pstat.memoryUtilization;
	astat.memoryUtilization += msg.memoryutilization();

	// Network
	astat.networkInUtilization -= pstat.networkInUtilization;
	astat.networkOutUtilization -= pstat.networkOutUtilization;
	astat.networkInUtilization += msg.networkinutilization();
	astat.networkOutUtilization += msg.networkoututilization();

	astat.numSamples += 1;
}

void Handler::_UpdateProcessStatistics(ProcessMessage &msg, StatBase &pstat)
{
	pstat.userCpuUtilization = msg.usercpuutilization();
	pstat.systemCpuUtilization = msg.systemcpuutilization();
	pstat.memoryUtilization = msg.memoryutilization();
	pstat.networkInUtilization = msg.networkinutilization();
	pstat.networkOutUtilization = msg.networkoututilization();

	pstat.userCpuUtilizationSum += pstat.userCpuUtilization;
	pstat.systemCpuUtilizationSum += pstat.systemCpuUtilization;
	pstat.memoryUtilizationSum += pstat.memoryUtilization;
	pstat.networkInUtilizationSum += pstat.networkInUtilization;
	pstat.networkOutUtilizationSum += pstat.networkOutUtilization;

	pstat.numSamples += 1;
}

void Handler::_RankTableItems()
{
//	LOG(INFO) << "Num proc names: " << _data->procNameMap.size();
	BOOST_FOREACH(ProcNameMap::value_type val, _data->procNameMap) {
		ProcName *procName = val.second;
		double newScore = procName->stat->userCpuUtilization + procName->stat->systemCpuUtilization;
		procName->visual->tableItem->score = newScore;
	}
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




