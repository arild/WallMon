/*
 * DataRouter.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: arild
 */

#include <string.h>
#include <stdlib.h> // For NULL
#include <glog/logging.h>
#include "DataRouter.h"


DataRouter::DataRouter()
{
	_handlers = *new handlerMap();
}

DataRouter::~DataRouter()
{
	//delete _handlers;
}

void DataRouter::RegisterHandler(IDataHandler &handler)
{
	Context *ctx = new Context();
	handler.OnInit(ctx);

	HandlerEvent event = *new HandlerEvent();
	event.handler = &handler;
	event.ctx = ctx;

	string key = ctx->key;
	_handlers[key] = event;
}

void DataRouter::Route(char *packet, int length)
{
	if (packet == NULL || length == 0) {
		LOG(ERROR) << "invalid packet";
		return;
	}
	string key = packet;
	int dataLength = length - key.length();
	void *data = (void *)&packet[key.length() + 1]; // +1 for skipping '\0' in string

	//LOG(INFO) << "Routing:  Key: " << key << "(" << key.length() << ") | " << "dataLength = "
	//		<< dataLength;
	HandlerEvent event = _handlers[key];
	event.handler->Handle(data, dataLength);
}
