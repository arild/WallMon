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
#define SAMPLE_FREQUENCY_MS 	40

void FastLoggerHandler::OnInit(Context *ctx)
{
	_ctx = ctx;
	_ctx->key = KEY;
}

void FastLoggerHandler::OnStop()
{
}

void FastLoggerHandler::Handle(void *data, int length)
{
}

void FastLoggerCollector::OnInit(Context *ctx)
{
	_ctx = ctx;
	_ctx->key = KEY;
	_ctx->sampleFrequencyMsec = SAMPLE_FREQUENCY_MS;

	_dataPacket = new FastLogger::DataPacket();
	_dataJunk = new char[SAMPLE_SIZE];
	_dataEncoded = new char[SAMPLE_SIZE * 2];
	memset(_dataJunk, 'a', SAMPLE_SIZE);
}

void FastLoggerCollector::OnStop()
{
	delete _dataPacket;
	delete _dataJunk;
	delete _dataEncoded;
}

int FastLoggerCollector::Sample(void **data)
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
