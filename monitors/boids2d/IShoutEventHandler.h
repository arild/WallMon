/*
 * IShoutEventHandler.h
 *
 *  Created on: May 15, 2011
 *      Author: arild
 */

#ifndef ISHOUTEVENTHANDLER_H_
#define ISHOUTEVENTHANDLER_H_

#include "ShoutMaster.h"

class IShoutEventHandler {
public:
	virtual ~IEntity() {}
	virtual void Handle(TouchEvent &event);
};


#endif /* ISHOUTEVENTHANDLER_H_ */
