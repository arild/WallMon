#include <glog/logging.h>
#include <boost/foreach.hpp>
#include "LocalSampler.h"

LocalSampler::LocalSampler()
{
	CHECK(_monitor.Open() == true);
}

LocalSampler::~LocalSampler()
{
}

void LocalSampler::Start()
{
	_running = true;
	_thread = boost::thread(&LocalSampler::_RunForever, this);
}

void LocalSampler::Stop()
{
	LOG(INFO) << "stopping local sampler...";
	_running = false;
	_thread.join();
	LOG(INFO) << "local sampler stopped";
}

vector<double> LocalSampler::GetNetworkUsageInMb()
{
	vector<double> usageMb;
	BOOST_FOREACH (unsigned int usageBytes, _networkUsageBytes)
		usageMb.push_back(usageBytes / (double)(1<<20));
	return usageMb;;
}

void LocalSampler::_RunForever()
{
	_monitor.Update();
	while (_running) {
		usleep(1000 * 970);
		LOG(INFO) << "UPDATE";
		_monitor.Update();
		_UpdateServerNetworkUsage();
	}
}

void LocalSampler::_UpdateServerNetworkUsage()
{
	unsigned int usage = (_monitor.GetNetworkInInBytes() + _monitor.GetNetworkOutInBytes());
	_networkUsageBytes.push_back(usage);
}
