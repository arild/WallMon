/*
 * Network.h
 *
 *  Created on: Jan 12, 2011
 *      Author: arild
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <map>
#include <boost/thread.hpp>
#include <ev++.h>
#include "PracticalSocket.h"
#include "DataRouter.h"
#include "IoLogger.h"

using namespace std;
typedef map<ev::io *, string> WatcherMap;

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
	static WatcherMap _watcherMap;
	static DataRouter *_router;
	static unsigned int _numConnectedClients;
	static IoLogger *_ioLogger;
	void _ServeForever();
	void _AcceptCallback(ev::io &watcher, int revents);
	void _ReadCallback(ev::io &watcher, int revents);
	void _DeleteAndCleanupWatcher(ev::io &watcher);
};

#endif /* NETWORK_H_ */
