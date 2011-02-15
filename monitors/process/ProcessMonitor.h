/*
 * ProcessMonitorHandler.h
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#ifndef PROCESSMONITORHANDLER_H_
#define PROCESSMONITORHANDLER_H_

#include "IDataHandler.h"
#include "IDataCollector.h"
#include "Protocol.pb.h"
#include "LinuxProcessMonitor.h"

class ProcessMonitorHandler: public IDataHandler {
public:
	virtual void OnStart();
	virtual void OnStop();
	virtual string GetKey();
	virtual void Handle(unsigned char *data, int length);
private:
	DataPacket *_dataPacket;
};

class ProcessMonitorCollector: public IDataCollector {
public:
	virtual void OnStart();
	virtual void OnStop();
	virtual string GetKey();
	virtual int GetScheduleIntervalInMsec();
	virtual int Sample(unsigned char **data);
private:
	LinuxProcessMonitor *_monitor;
	DataPacket *_dataPacket;
	unsigned char *_buffer;
};

#endif /* PROCESSMONITORHANDLER_H_ */
