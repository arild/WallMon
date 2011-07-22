#ifndef LocalSampler_H_
#define LocalSampler_H_

#include <boost/thread.hpp>
#include <vector>
#include "LinuxProcessMonitorLight.h"
using namespace std;

class LocalSampler {
public:
	LocalSampler();
	virtual ~LocalSampler();
	void Start();
	void Stop();
	vector<double> GetNetworkUsageInMb();
private:
	boost::thread _thread;
	bool _running;
	LinuxProcessMonitorLight _monitor;
	vector<unsigned int> _networkUsageBytes;
	void _RunForever();
	void _UpdateServerNetworkUsage();
};

#endif /* LocalSampler_H_ */
