/*
 * Wallmon.h
 *
 *  Created on: Feb 22, 2011
 *      Author: arild
 */

#ifndef WALLMON_H_
#define WALLMON_H_

#include <string>
#include "Wallmon.pb.h"

using std::string;

class Context {
public:
	string server;
	string key;
	int sampleFrequencyMsec;
	bool concurrentExecution;

	Context()
	{
		server = "";
		key = "";
		sampleFrequencyMsec = 1000;
		concurrentExecution = false;
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
