/*
 * DataRouter.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: arild
 */

#include <string.h>
#include <glog/logging.h>
#include "DataRouter.h"

DataRouter::DataRouter()
{
	_handlers = new handlerMap();
}

DataRouter::~DataRouter()
{
}

void DataRouter::RegisterHandler(IDataHandler *handler)
{
	handler->OnStart();
	string key = handler->GetKey();
	(*_handlers)[key] = handler;
	LOG(INFO) << "Handler registered: Key=" << key;
}

void DataRouter::Route(char *packet, int length)
{
	string key = packet;
	int dataLength = length - key.length();
	unsigned char *data = (unsigned char *) &packet[key.length() + 1]; // +1 for skipping '\0' in string

	LOG(INFO) << "Routing:  Key = " << key << " | dataLength = " << dataLength;
	IDataHandler *handler = (*_handlers)[key];
	handler->Handle(data, dataLength);
}
