/*
 * IDataHandler.h
 *
 *  Created on: Jan 31, 2011
 *      Author: arild
 */

#ifndef IDATAHANDLER_H_
#define IDATAHANDLER_H_

#include <string>

using std::string;

class IDataHandler {
public:
	virtual void OnStart() = 0;
	virtual void OnStop() = 0;
	virtual string GetKey() = 0;
	virtual void Handle(unsigned char *data, int length) = 0;
};

// the types of the class factories
typedef IDataHandler *create_handler_t();
typedef void destroy_handler_t(IDataHandler *);

#endif /* IDATAHANDLER_H_ */
