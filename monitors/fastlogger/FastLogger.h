/*
 * FastLogger.h
 *
 *  Created on: Feb 16, 2011
 *      Author: arild
 */

#ifndef FASTLOGGER_H_
#define FASTLOGGER_H_

#include "Wallmon.h"
#include "FastLoggerProtocol.pb.h"

class FastLoggerHandler: public IDataHandler {
public:
	virtual void OnInit(Context *ctx);
	virtual void OnStop();
	virtual void Handle(void *data, int length);
private:
	Context *_ctx;
	FastLogger::DataPacket *_dataPacket;
};

class FastLoggerCollector: public IDataCollector {
public:
	virtual void OnInit(Context *ctx);
	virtual void OnStop();
	virtual int Sample(void **data);
private:
	Context *_ctx;
	FastLogger::DataPacket *_dataPacket;
	char *_dataJunk;
	char *_dataEncoded;
};

#endif /* FASTLOGGER_H_ */
