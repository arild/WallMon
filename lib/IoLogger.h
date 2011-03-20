/*
 * IoLogger.h
 *
 *  Created on: Mar 17, 2011
 *      Author: arild
 */

#ifndef IOLOGGER_H_
#define IOLOGGER_H_

class IoLogger {
public:
	IoLogger(unsigned long int triggerInterval);
	virtual ~IoLogger();
	void Read(int numBytes);
	void Write(int numBytes);
private:
	unsigned long int _numBytesLogTriggerInterval;
	unsigned long int _totalRead;
	unsigned long int _readTrigger;

	unsigned long int _totalWrite;
	unsigned long int _writeTrigger;

	unsigned long int _MB;
};

#endif /* IOLOGGER_H_ */
