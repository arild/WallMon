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
#include "Wallmon.pb.h"
#include "Wallmon.h"

class HandlerEvent {
public:
	IDataHandler *handler;
	Context *ctx;
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

class DataRouter {
public:
	DataRouter();
	virtual ~DataRouter();
	void Start();
	void Stop();
	void RegisterHandler(IDataHandler &handler);
	void Route(char *message, int length);
private:
	boost::thread _thread;
	bool _running;
	Queue<RouterItem *> *_queue;
	handlerMap *_handlers;
	WallmonMessage *_msg;
	void _RouteForever();
};

#endif /* DATAROUTER_H_ */
