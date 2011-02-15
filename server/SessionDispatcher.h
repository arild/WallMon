/*
 * Dispatcher.h
 *
 *  Created on: Jan 23, 2011
 *      Author: arild
 */

#ifndef DISPATCHER_H_
#define DISPATCHER_H_

#include <string>
#include <boost/thread.hpp>
#include "PracticalSocket.h"

using std::string;

class SessionDispatcher {
public:
	SessionDispatcher();
	virtual ~SessionDispatcher();
	void Dispatch(string filePath);
private:
	UDPSocket *_socket;
};

#endif /* DISPATCHER_H_ */
