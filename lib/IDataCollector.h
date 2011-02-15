/*
 * IDataCollector.h
 *
 *  Created on: Jan 28, 2011
 *      Author: arild
 */

#ifndef IDATACOLLECTOR_H_
#define IDATACOLLECTOR_H_

#include <string>

using std::string;

class IDataCollector {
public:
	virtual void OnStart() = 0;
	virtual void OnStop() = 0;
	virtual string GetKey() = 0;
	virtual int GetScheduleIntervalInMsec() = 0;
	virtual int Sample(unsigned char **data) = 0;
};

// the types of the class factories
typedef IDataCollector *create_collector_t();
typedef void destroy_collector_t(IDataCollector *);

#endif /* IDATACOLLECTOR_H_ */
