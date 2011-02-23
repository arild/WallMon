/*
 * DataRouter.h
 *
 *  Created on: Jan 31, 2011
 *      Author: arild
 */

#ifndef DATAROUTER_H_
#define DATAROUTER_H_

#include <string>
//#include <boost/unordered_map.hpp>
#include <map>
#include "Wallmon.h"

class HandlerEvent {
public:
	IDataHandler *handler;
	Context *ctx;
};

typedef std::map<string, HandlerEvent> handlerMap;

class DataRouter {
public:
	DataRouter();
	virtual ~DataRouter();
	void RegisterHandler(IDataHandler &handler);
	void Route(char *packet, int length);
private:
	handlerMap _handlers;
};

#endif /* DATAROUTER_H_ */
