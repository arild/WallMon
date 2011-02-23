/*
 * SharedLibraryWatcher.cpp
 *
 *  Created on: Feb 8, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "Config.h"
#include "SharedLibraryWatcher.h"
#include "ClassLoader.h"

using namespace boost::filesystem;

SharedLibraryWatcher::SharedLibraryWatcher(DataRouter *router, SessionDispatcher *dispatcher)
{
	_router = router;
	_dispatcher = dispatcher;

	string filePath = SHARED_LIBRARY_PATH;
	if (is_directory(filePath)) {
		for (directory_iterator itr(filePath); itr != directory_iterator(); ++itr) {
			string fileName = itr->path().filename();
			int fileSize = -1;
			if (is_regular_file(itr->status()))
				fileSize = file_size(itr->path());
			LOG(INFO) << "shared library found: filename=" << fileName << " | size="
					<< fileSize << " bytes";
			_HandleSharedLibrary(filePath + fileName);
		}
	}
}

SharedLibraryWatcher::~SharedLibraryWatcher()
{
}

void SharedLibraryWatcher::_HandleSharedLibrary(string filePath)
{
	// TODO: Fix memory-leak. (Problem related to closing descriptor used by the class loader)
	IDataHandler *handler;
	try {
		ClassLoader *loader = new ClassLoader(filePath);
		handler = loader->LoadAndInstantiateHandler();
	} catch (exception &e) {
		LOG(ERROR) << "failed loading and instatiating handler: " << e.what();
		return;
	}
	_router->RegisterHandler(*handler);
	_dispatcher->Dispatch(filePath);
}

