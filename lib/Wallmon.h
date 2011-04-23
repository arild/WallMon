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
#include "Wallmon.pb.h"

using namespace std;

class Context {
public:
	string server;
	vector<string> *servers;
	string key;
	int sampleFrequencyMsec;
	bool concurrentExecution;

	Context()
	{
		server = "";
		servers = new vector<string>();
		key = "";
		sampleFrequencyMsec = 1000;
		concurrentExecution = false;
	}

	~Context()
	{
		delete servers;
	}

	void AddServer(string serverAddress)
	{
		LOG(INFO) << "New Server: " << serverAddress;
		servers->push_back(serverAddress);
	}

	void AddServers(vector<string> serverAddresses)
	{
		for (int i = 0; i < serverAddresses.size(); i++)
			AddServer(serverAddresses[i]);
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
