/*
 * SlowLogger.cpp
 *
 *  Created on: Feb 16, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include "SlowLogger.h"

#define KEY						"SLOW_LOGGER"
#define SAMPLE_SIZE				1024
#define SAMPLE_FREQUENCY_MS 	500

int cnt = 0;
void SlowLoggerHandler::OnInit(Context *ctx)
{
	_ctx = ctx;
	_ctx->key = KEY;
}

void SlowLoggerHandler::OnStop()
{
}

void SlowLoggerHandler::Handle(void *data, int length)
{
	int i;
	for (i = 0; i < 10000; i++)
		;
}

void SlowLoggerCollector::OnInit(Context *ctx)
{
	_ctx = ctx;
	_ctx->key = KEY;
	_ctx->sampleFrequencyMsec = SAMPLE_FREQUENCY_MS;

	_dataPacket = new SlowLogger::DataPacket();
	_dataJunk = new char[SAMPLE_SIZE];
	_dataEncoded = new char[SAMPLE_SIZE * 2];
	memset(_dataJunk, 'a', SAMPLE_SIZE);
}

void SlowLoggerCollector::OnStop()
{
	delete _dataPacket;
	delete _dataJunk;
	delete _dataEncoded;
}

int SlowLoggerCollector::Sample(void **data)
{
	//if (++cnt % 10 == 0)
	//	_ctx->sampleFrequencyMsec--;

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
