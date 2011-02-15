/*
 * SharedLibraryWatcher.h
 *
 *  Created on: Feb 8, 2011
 *      Author: arild
 */

#ifndef SHAREDLIBRARYWATCHER_H_
#define SHAREDLIBRARYWATCHER_H_

#include "DataRouter.h"
#include "SessionDispatcher.h"

class SharedLibraryWatcher {
public:
	SharedLibraryWatcher(DataRouter *router, SessionDispatcher *dispatcher);
	virtual ~SharedLibraryWatcher();

private:
	DataRouter *_router;
	SessionDispatcher *_dispatcher;
	void _HandleSharedLibrary(string filePath);
};

#endif /* SHAREDLIBRARYWATCHER_H_ */
