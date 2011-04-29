/*
 * IEventHandler.h
 *
 *  Created on: Apr 20, 2011
 *      Author: arild
 */

#ifndef IEVENTHANDLER_H_
#define IEVENTHANDLER_H_

#include "Wallmon.h"

class IMonitorManager {
public:
	virtual ~IMonitorManager(){}
	virtual void Register(IBase &monitor, Context &ctx) = 0;
	virtual void Remove(IBase &monitor) = 0;
	virtual void Event(IBase &monitor, char *msg) = 0;
};

#endif /* IEVENTHANDLER_H_ */
