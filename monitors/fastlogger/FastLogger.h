/*
 * FastLogger.h
 *
 *  Created on: Feb 16, 2011
 *      Author: arild
 */

#ifndef FASTLOGGER_H_
#define FASTLOGGER_H_

#include "IDataHandler.h"
#include "IDataCollector.h"
#include "FastLoggerProtocol.pb.h"

class FastLoggerHandler: public IDataHandler {
public:
	virtual void OnStart();
	virtual void OnStop();
	virtual string GetKey();
	virtual void Handle(unsigned char *data, int length);
private:
	FastLogger::DataPacket *_dataPacket;
};

class FastLoggerCollector: public IDataCollector {
public:
	virtual void OnStart();
	virtual void OnStop();
	virtual string GetKey();
	virtual int GetScheduleIntervalInMsec();
	virtual int Sample(unsigned char **data);
private:
	FastLogger::DataPacket *_dataPacket;
	unsigned char *_dataJunk;
	unsigned char *_dataEncoded;
};

#endif /* FASTLOGGER_H_ */
