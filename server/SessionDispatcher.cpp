/*
 * Dispatcher.cpp
 *
 *  Created on: Jan 23, 2011
 *      Author: arild
 */
#include <glog/logging.h>
#include "Config.h"
#include "SessionDispatcher.h"

using namespace std;

SessionDispatcher::SessionDispatcher()
{
	unsigned char multicastTTL = 3; // Default TTL
	try {
		_socket = new UDPSocket();
		_socket->setMulticastTTL(multicastTTL);
	} catch (exception &e) {
		LOG(ERROR) << "failed creating multicast socket: " << e.what();
	}
}

SessionDispatcher::~SessionDispatcher()
{
	_socket->disconnect();
	delete _socket;
}

void SessionDispatcher::Dispatch(string fileName)
{
	string msg = PROTOCOL_PREFIX;
	msg += fileName;
	try {
		LOG(INFO) << "Dispatching new session: " << fileName;
		//_socket->sendTo(msg.c_str(), msg.length(), MULITCAST_ADDRESS, MULTICAST_LISTEN_PORT);
	} catch (exception &e) {
		LOG(ERROR) << "failed dispatching on multicast socket: " << e.what();
	}
}

