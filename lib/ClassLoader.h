/*
 * ClassLoader.h
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#ifndef CLASSLOADER_H_
#define CLASSLOADER_H_

#include <string>
#include "Wallmon.h"

using std::string;

class ClassLoader {
public:
	ClassLoader(string filePath);
	virtual ~ClassLoader();
	IBase *LoadAndInstantiateHandler();
	IBase *LoadAndInstantiateCollector();

private:
	void *_dl_handle;
};

#endif /* CLASSLOADER_H_ */
