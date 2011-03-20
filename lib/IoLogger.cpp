/*
 * IoLogger.cpp
 *
 *  Created on: Mar 17, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include "IoLogger.h"


IoLogger::IoLogger(unsigned long int triggerInterval)
{
	_numBytesLogTriggerInterval = triggerInterval;
	_totalRead = _totalWrite = 0;
	_readTrigger = _writeTrigger = _numBytesLogTriggerInterval;
	_MB = 1024 * 1000;
}

IoLogger::~IoLogger()
{
}

void IoLogger::Read(int numBytes)
{
	_totalRead += numBytes;
	if (_totalRead >= _readTrigger) {
		_readTrigger += _numBytesLogTriggerInterval;
		unsigned long int tmp = _readTrigger / _MB;
		LOG(INFO) << tmp << " MBs received";
	}
}

void IoLogger::Write(int numBytes)
{
	_totalWrite += numBytes;
	if (_totalWrite >= _writeTrigger) {
		_writeTrigger += _numBytesLogTriggerInterval;
		unsigned long int tmp = _writeTrigger / _MB;
		LOG(INFO) << tmp << " MBs sent";
	}
}
