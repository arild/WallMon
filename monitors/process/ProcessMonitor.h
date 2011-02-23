/*
 * ProcessMonitorHandler.h
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#ifndef PROCESSMONITORHANDLER_H_
#define PROCESSMONITORHANDLER_H_

#include "Wallmon.h"
#include "Protocol.pb.h"
#include "LinuxProcessMonitor.h"

class ProcessMonitorHandler: public IDataHandler {
public:
	virtual void OnInit(Context *ctx);
	virtual void OnStop();
	virtual void Handle(void *data, int length);
private:
	DataPacket *_dataPacket;
};

class ProcessMonitorCollector: public IDataCollector {
public:
	virtual void OnInit(Context *ctx);
	virtual void OnStop();
	virtual int Sample(void **data);
private:
	LinuxProcessMonitor *_monitor;
	DataPacket *_dataPacket;
	char *_buffer;
};

#endif /* PROCESSMONITORHANDLER_H_ */
