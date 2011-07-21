/*
 * Wallmon.h
 *
 *  Created on: Feb 22, 2011
 *      Author: arild
 */

#ifndef WALLMON_H_
#define WALLMON_H_

#include <string>
#include <vector>
#include <glog/logging.h>
#include <boost/tuple/tuple.hpp>
#include "Config.h"
#include "stubs/Wallmon.pb.h"

using namespace std;
using namespace boost::tuples;

class Context {
public:
	string key;
	vector< tuple<string, int> > servers;
	int sampleFrequencyMsec;
	bool concurrentExecution;
	bool includeStatistics;

	Context()
	{
		key = "";
		sampleFrequencyMsec = 1000;
		concurrentExecution = false;
		includeStatistics = false;
	}

	~Context()
	{
	}

	void AddServer(tuple<string, int> serverAddress)
	{
		LOG(INFO) << "Adding server: " << serverAddress.get<0>() << " | " << serverAddress.get<1>();
		servers.push_back(serverAddress);
	}

	void AddServer(string serverAddress, int port)
	{
		AddServer(make_tuple(serverAddress, port));
	}

	void AddServer(string serverAddress)
	{
		AddServer(serverAddress, STREAMER_ENTRY_PORT);
	}

	void AddServers(vector< tuple<string, int> > serverAddresses)
	{
		for (int i = 0; i < serverAddresses.size(); i++)
			AddServer(serverAddresses[i]);
	}

	void AddServers(vector<string> serverAddresses)
	{
		for (int i = 0; i < serverAddresses.size(); i++)
			AddServer(serverAddresses[i], STREAMER_ENTRY_PORT);
	}


};

class IBase {
public:
	virtual ~IBase() {}
	virtual void OnInit(Context *ctx) = 0;
	virtual void OnStop() = 0;
};


class IDataHandler : virtual public IBase {
public:
	virtual ~IDataHandler() {}
	virtual void Handle(void *data, int length) = 0;
};

class IDataCollector : virtual public IBase {
public:
	virtual ~IDataCollector() {}
	virtual int Sample(void **data) = 0;
};


class IDataHandlerProtobuf : virtual public IBase {
public:
	virtual ~IDataHandlerProtobuf() {}
	virtual void Handle(WallmonMessage *msg) = 0;
};

class IDataCollectorProtobuf : virtual public IBase {
public:
	virtual ~IDataCollectorProtobuf() {}
	virtual void Sample(WallmonMessage *msg) = 0;
};


// the types of the class factories
typedef IBase *create_handler_t();
typedef void destroy_handler_t(IBase *);
typedef IBase *create_collector_t();
typedef void destroy_collector_t(IBase*);

#endif /* WALLMON_H_ */
