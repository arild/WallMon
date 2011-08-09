/*
 * DataRouter.h
 *
 *  Created on: Jan 31, 2011
 *      Author: arild
 */

#ifndef DATAROUTER_H_
#define DATAROUTER_H_

#include <string>
#include <map>
#include <boost/thread.hpp>
#include "Queue.h"
#include "stubs/Wallmon.pb.h"
#include "Wallmon.h"
#include "IMonitorManager.h"

class HandlerEvent {
public:
	Context *ctx;
	IDataHandler *handler;
	IDataHandlerProtobuf *handlerProtobuf;

	HandlerEvent()
	{
		handler = NULL;
		handlerProtobuf = NULL;
	}
};

class RouterItem {
public:
	int length;
	char *message;

	RouterItem(int messageLength)
	{
		message = new char[messageLength];
		length = messageLength;
	}

	~RouterItem()
	{
		delete message;
	}
};

typedef std::map<string, HandlerEvent *> handlerMap;

class DataRouter : IMonitorManager {
public:
	DataRouter();
	virtual ~DataRouter();
	void Start();
	void Stop();
	virtual void Register(IBase &monitor, Context &ctx);
	virtual void Remove(IBase &monitor);
	virtual void Event(IBase &monitor, char *msg);
	void Route(char *message, int length);
private:
	boost::thread _thread;
	bool _running;
	Queue<RouterItem *> *_queue;
	handlerMap *_handlers;
	WallmonMessage *_msg;
	unsigned int _messageNumber;
	void _RouteForever();
};

#endif /* DATAROUTER_H_ */
