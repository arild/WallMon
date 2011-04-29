/**
 * @file   DataRouter.cpp
 * @Author Arild Nilsen
 * @date   January, 2011
 *
 * Parses messages from clients and invokes related handlers.
 */

#include <iostream>
#include <string.h>
#include <stdlib.h> // For NULL
#include <glog/logging.h>
#include "DataRouter.h"
#include <typeinfo>

DataRouter::DataRouter()
{
	_queue = new Queue<RouterItem *> (100);
	_handlers = new handlerMap();
	_msg = new WallmonMessage();
}

DataRouter::~DataRouter()
{
	delete _queue;
	delete _handlers;
	delete _msg;
}

void DataRouter::Start()
{
	_running = true;
	_thread = boost::thread(&DataRouter::_RouteForever, this);
}

/**
 * Stops the streamer thread by changing the thread's termination flag and
 * then putting a dummy item on the queue that the thread listen on.
 */
void DataRouter::Stop()
{
	LOG(INFO) << "stopping DataRouter...";
	_running = false;
	RouterItem item(1);
	_queue->Push(&item);
	_thread.join();
	LOG(INFO) << "DataRouter stopped";
}

void DataRouter::Route(char *message, int length)
{
	if (message == NULL || length == 0) {
		LOG(ERROR) << "invalid message";
		return;
	}
	RouterItem *item = new RouterItem(length);
	memcpy(item->message, message, length);
	_queue->Push(item);
}

void DataRouter::Register(IBase & monitor, Context & ctx)
{
	// Save and determine type of handler: the dynamic_cast() method
	// will return NULL if the specified cast type does not match.
	HandlerEvent *event = new HandlerEvent();
	event->ctx = &ctx;
	event->handler = dynamic_cast<IDataHandler *>(&monitor);
	event->handlerProtobuf = dynamic_cast<IDataHandlerProtobuf *>(&monitor);

	string key = ctx.key;
	(*_handlers)[key] = event;
}

void DataRouter::Remove(IBase & monitor)
{
}

void DataRouter::Event(IBase & monitor, char *msg)
{
}

void DataRouter::_RouteForever()
{
	LOG(INFO) << "DataRouter started and serving requests";
	while (true) {
		RouterItem *item = _queue->Pop();
		if (_running == false)
			break;
		_msg->ParseFromArray(item->message, item->length);

		// Retrieve and invoke associated handler
		HandlerEvent *event = (*_handlers)[_msg->key()];
		if (event->handler)
			event->handler->Handle((void *)_msg->data().c_str(), _msg->data().length());
		else if (event->handlerProtobuf)
			event->handlerProtobuf->Handle(_msg);
		else
			LOG(FATAL) << "unknown handler type";

		delete item;
	}
}

