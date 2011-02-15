/*
 * Dispatcher.h
 *
 *  Created on: Jan 23, 2011
 *      Author: arild
 */

#ifndef DISPATCHER_H_
#define DISPATCHER_H_

#include <boost/thread.hpp>
#include "PracticalSocket.h" // For UDPSocket and SocketException
#include "Scheduler.h"
#include "Streamer.h"

class Dispatcher {
public:
	Dispatcher(Scheduler *scheduler);
	virtual ~Dispatcher();
	void Start();
	void Stop();
private:
	boost::thread _thread;
	bool _running;
	Scheduler *_scheduler;
	UDPSocket *_socket;
	void _ListenForever();
};

#endif /* DISPATCHER_H_ */
