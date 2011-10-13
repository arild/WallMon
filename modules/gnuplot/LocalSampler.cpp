#include <glog/logging.h>
#include <boost/foreach.hpp>
#include "LocalSampler.h"

LocalSampler::LocalSampler(vector<int> intervalTimeMsec)
{
	CHECK(_monitor.Open() == true);
	_intervalTimeMsec = intervalTimeMsec;
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
	while (_running && _intervalTimeMsec.size() > 0) {
		int intervalMsec = _intervalTimeMsec[0];
		usleep(intervalMsec * 1000);
		LOG(INFO) << "UPDATE";
		_monitor.Update();
		_intervalTimeMsec.erase(_intervalTimeMsec.begin());

		unsigned int avgPerSec = (unsigned int)(_monitor.GetIoInBytesouble)(intervalMsec / (double)1000));
		_networkUsageBytes.push_back(avgPerSec);
	}
}
