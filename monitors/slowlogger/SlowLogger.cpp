/*
 * SlowLogger.cpp
 *
 *  Created on: Feb 16, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include "SlowLogger.h"

#define KEY					"SLOWLOGGER"
#define SAMPLE_SIZE			4000
#define SAMPLE_INTERVAL_MS 	1000

void SlowLoggerHandler::OnStart()
{
}

void SlowLoggerHandler::OnStop()
{
}

string SlowLoggerHandler::GetKey()
{
	return KEY;
}

void SlowLoggerHandler::Handle(unsigned char *data, int length)
{
}

void SlowLoggerCollector::OnStart()
{
	_dataPacket = new SlowLogger::DataPacket();
	_dataJunk = new unsigned char[SAMPLE_SIZE];
	_dataEncoded = new unsigned char[SAMPLE_SIZE * 2];
	memset(_dataJunk, 'a', SAMPLE_SIZE);
}

void SlowLoggerCollector::OnStop()
{
	delete _dataPacket;
	delete _dataJunk;
	delete _dataEncoded;
}

string SlowLoggerCollector::GetKey()
{
	return KEY;
}

int SlowLoggerCollector::GetScheduleIntervalInMsec()
{
	return SAMPLE_INTERVAL_MS;
}

int SlowLoggerCollector::Sample(unsigned char **data)
{
	_dataPacket->set_junk(_dataJunk, SAMPLE_SIZE);
	_dataPacket->SerializeToArray(_dataEncoded, SAMPLE_SIZE);
	*data = _dataEncoded;
	return _dataPacket->ByteSize();
}

// class factories - needed to bootstrap object orientation with dlfcn.h
extern "C" SlowLoggerHandler *create_handler()
{
	return new SlowLoggerHandler;
}

extern "C" void destroy_handler(SlowLoggerHandler *p)
{
	delete p;
}

extern "C" SlowLoggerCollector *create_collector()
{
	return new SlowLoggerCollector;
}

extern "C" void destroy_collector(SlowLoggerCollector *p)
{
	delete p;
}
