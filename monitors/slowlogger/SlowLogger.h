/*
 * SlowLogger.h
 *
 *  Created on: Feb 16, 2011
 *      Author: arild
 */

#ifndef SLOWLOGGER_H_
#define SLOWLOGGER_H_

#include "Wallmon.h"
#include "SlowLoggerProtocol.pb.h"

class SlowLoggerHandler: public IDataHandler {
public:
	virtual void OnInit(Context *ctx);
	virtual void OnStop();
	virtual void Handle(void *data, int length);
private:
	Context *_ctx;
	SlowLogger::DataPacket *_dataPacket;
};

class SlowLoggerCollector: public IDataCollector {
public:
	virtual void OnInit(Context *ctx);
	virtual void OnStop();
	virtual int Sample(void **data);
private:
	Context *_ctx;
	SlowLogger::DataPacket *_dataPacket;
	char *_dataJunk;
	char *_dataEncoded;
};

#endif /* SLOWLOGGER_H_ */
