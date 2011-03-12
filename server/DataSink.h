/*
 * Network.h
 *
 *  Created on: Jan 12, 2011
 *      Author: arild
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <set>
#include <boost/thread.hpp>
#include <ev++.h>
#include "PracticalSocket.h"
#include "DataRouter.h"

using namespace std;

class DataSink {
public:
	DataSink(DataRouter *router);
	virtual ~DataSink();
	void Start();
	void Stop();

private:
	boost::thread _thread;
	struct ev_loop *_loop;
	int _acceptSockfd;
	set<ev::io *> *_watcherSet;
	static DataRouter *_router;
	static unsigned int _numConnectedClients;
	static unsigned int _totalNumBytesReceived;
	static unsigned int _numBytesLogTrigger;
	void _ServeForever();
	void _AcceptCallback(ev::io &watcher, int revents);
	void _ReadCallback(ev::io &watcher, int revents);
	void _DeleteAndCleanupWatcher(ev::io &watcher);
};

#endif /* NETWORK_H_ */
