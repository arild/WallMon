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

void DataRouter::RegisterHandler(IDataHandler *handler)
{
	handler->OnStart();
	string key = handler->GetKey();
	_handlers[key] = handler;
}

void DataRouter::Route(char *packet, int length)
{
	if (packet == NULL || length == 0) {
		LOG(ERROR) << "invalid packet";
		return;
	}
	string key = packet;
	int dataLength = length - key.length();
	unsigned char *data = (unsigned char *) &packet[key.length() + 1]; // +1 for skipping '\0' in string

	LOG(INFO) << "Routing:  Key: " << key << "(" << key.length() << ") | " << "dataLength = "
			<< dataLength;
	IDataHandler *handler = _handlers[key];
	handler->Handle(data, dataLength);
}
