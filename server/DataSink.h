/*
 * Network.h
 *
 *  Created on: Jan 12, 2011
 *      Author: arild
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <boost/thread.hpp>
#include <ev++.h>
#include "PracticalSocket.h"
#include <boost/unordered_set.hpp>

#include "DataRouter.h"

class DataSink {
public:
	DataSink(DataRouter *router);
	virtual ~DataSink();
	void Start();
	void Stop();

private:
	DataRouter *_router;
	boost::thread _thread;
	bool _running;
	struct ev_loop *_loop;
	int _socketDescriptor;
	boost::unordered_set<ev::io *> *_watcherSet;
	int _numConnectedClients;
	void _ServeForever();
	void _AcceptCallback(ev::io &watcher, int revents);
	void _ReadCallback(ev::io &watcher, int revents);
	void _DeleteWatcher(ev::io &watcher);
};

#endif /* NETWORK_H_ */
