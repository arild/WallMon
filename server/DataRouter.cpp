/**
 * @file   DataRouter.cpp
 * @Author Arild Nilsen
 * @date   January, 2011
 *
 * Parses messages from clients and invokes related handlers.
 */

#include <string.h>
#include <stdlib.h> // For NULL
#include <glog/logging.h>
#include "DataRouter.h"

DataRouter::DataRouter()
{
	_queue = new Queue<RouterItem *> (100);
	_handlers = new handlerMap();
}

DataRouter::~DataRouter()
{
	delete _queue;
	delete _handlers;
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

void DataRouter::RegisterHandler(IDataHandler &handler)
{
	Context *ctx = new Context();
	handler.OnInit(ctx);

	HandlerEvent *event = new HandlerEvent();
	event->handler = &handler;
	event->ctx = ctx;

	string key = ctx->key;
	(*_handlers)[key] = event;
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

void DataRouter::_RouteForever()
{
	LOG(INFO) << "DataRouter started and serving requests";
	while (true) {
		RouterItem *item = _queue->Pop();
		if (_running == false)
			break;

		string key = item->message;
		int totalKeyLength = key.length() + 1; // +1 for skipping '\0' in string
		int dataLength = item->length - totalKeyLength;
		void *data = (void *) &item->message[totalKeyLength];

		HandlerEvent *event = (*_handlers)[key];
		event->handler->Handle(data, dataLength);

		delete item;
	}
}

