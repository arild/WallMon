/*
 * SlowLogger.h
 *
 *  Created on: Feb 16, 2011
 *      Author: arild
 */

#ifndef SLOWLOGGER_H_
#define SLOWLOGGER_H_

#include "IDataHandler.h"
#include "IDataCollector.h"
#include "SlowLoggerProtocol.pb.h"

class SlowLoggerHandler: public IDataHandler {
public:
	virtual void OnStart();
	virtual void OnStop();
	virtual string GetKey();
	virtual void Handle(unsigned char *data, int length);
private:
	SlowLogger::DataPacket *_dataPacket;
};

class SlowLoggerCollector: public IDataCollector {
public:
	virtual void OnStart();
	virtual void OnStop();
	virtual string GetKey();
	virtual int GetScheduleIntervalInMsec();
	virtual int Sample(unsigned char **data);
private:
	SlowLogger::DataPacket *_dataPacket;
	unsigned char *_dataJunk;
	unsigned char *_dataEncoded;
};

#endif /* SLOWLOGGER_H_ */
