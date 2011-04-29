/*
 * Dispatcher.h
 *
 *  Created on: Jan 23, 2011
 *      Author: arild
 */

#ifndef MONITORDISPATCHER_H_
#define MONITORDISPATCHER_H_

#include <boost/thread.hpp>
#include "PracticalSocket.h" // For UDPSocket and SocketException
#include "IMonitorManager.h"
#include <map>
using namespace std;

class MonitorDispatcher {
public:
	MonitorDispatcher(IMonitorManager &manager, int multicastListenPort);
	virtual ~MonitorDispatcher();
	void Start();
	void Stop();
private:
	boost::thread _thread;
	bool _running;
	UDPSocket *_socket;
	IMonitorManager *_monitorManager;
	int _multicastListenPort;
	map<string, IBase *> _monitors;
	void _ListenForever();
};

#endif /* MONITORDISPATCHER_H_ */
