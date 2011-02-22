/*
 * FastLogger.cpp
 *
 *  Created on: Feb 16, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include "FastLogger.h"

#define KEY					"FAST_LOGGER"
#define SAMPLE_SIZE			913
#define SAMPLE_INTERVAL_MS 	40

void FastLoggerHandler::OnStart()
{
}

void FastLoggerHandler::OnStop()
{
}

string FastLoggerHandler::GetKey()
{
	return KEY;
}

void FastLoggerHandler::Handle(unsigned char *data, int length)
{
}

void FastLoggerCollector::OnStart()
{
	_dataPacket = new FastLogger::DataPacket();
	_dataJunk = new unsigned char[SAMPLE_SIZE];
	_dataEncoded = new unsigned char[SAMPLE_SIZE * 2];
	memset(_dataJunk, 'a', SAMPLE_SIZE);
}

void FastLoggerCollector::OnStop()
{
	delete _dataPacket;
	delete _dataJunk;
	delete _dataEncoded;
}

string FastLoggerCollector::GetKey()
{
	return KEY;
}

int FastLoggerCollector::GetScheduleIntervalInMsec()
{
	return SAMPLE_INTERVAL_MS;
}

int FastLoggerCollector::Sample(unsigned char **data)
{
	_dataPacket->set_junk(_dataJunk, SAMPLE_SIZE);
	_dataPacket->SerializeToArray(_dataEncoded, SAMPLE_SIZE);
	*data = _dataEncoded;
	return _dataPacket->ByteSize();
}


// class factories - needed to bootstrap object orientation with dlfcn.h
extern "C" FastLoggerHandler *create_handler()
{
	return new FastLoggerHandler;
}

extern "C" void destroy_handler(FastLoggerHandler *p)
{
	delete p;
}

extern "C" FastLoggerCollector *create_collector()
{
	return new FastLoggerCollector;
}

extern "C" void destroy_collector(FastLoggerCollector *p)
{
	delete p;
}
