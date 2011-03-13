/*
 * ClassLoader.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#include <dlfcn.h>
#include <iostream>
#include <glog/logging.h>
#include "Config.h"
#include "ClassLoader.h"

ClassLoader::ClassLoader(string filePath)
{
	/* Open the shared object */
	_dl_handle = dlopen(filePath.c_str(), RTLD_LAZY);
	if (!_dl_handle)
		LOG(ERROR) << "dlopen() failed: " << dlerror();
}

ClassLoader::~ClassLoader()
{
	//dlclose(_dl_handle);
}

IBase *ClassLoader::LoadAndInstantiateHandler()
{
	/* Resolve the symbol (method) from the object */
	create_handler_t *create_handler = (create_handler_t*) dlsym(_dl_handle, "create_handler");
	char *error = dlerror();
	if (error != NULL)
		LOG(ERROR) << "dlsym() failed: " << dlerror();
	IBase *handler = (IBase *) create_handler();
	return handler;
}

IBase *ClassLoader::LoadAndInstantiateCollector()
{
	/* Resolve the symbol (method) from the object */
	create_collector_t *create_collector = (create_collector_t*) dlsym(_dl_handle,
			"create_collector");
	char *error = dlerror();
	if (error != NULL)
		LOG(ERROR) << "dlsym() failed(): " << dlerror();
	IBase *handler = (IBase *) create_collector();
	return handler;
}

