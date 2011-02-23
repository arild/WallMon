/*
 * Wallmon.h
 *
 *  Created on: Feb 22, 2011
 *      Author: arild
 */

#ifndef WALLMON_H_
#define WALLMON_H_

#include <string>

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

class IDataHandler {
public:
	virtual ~IDataHandler() {}
	virtual void OnInit(Context *ctx) = 0;
	virtual void OnStop() = 0;
	virtual void Handle(void *data, int length) = 0;
};

class IDataCollector {
public:
	virtual ~IDataCollector() {}
	virtual void OnInit(Context *ctx) = 0;
	virtual void OnStop() = 0;
	virtual int Sample(void **data) = 0;
};

// the types of the class factories
typedef IDataHandler *create_handler_t();
typedef void destroy_handler_t(IDataHandler *);
typedef IDataCollector *create_collector_t();
typedef void destroy_collector_t(IDataCollector *);

#endif /* WALLMON_H_ */
