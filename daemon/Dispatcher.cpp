/*
 * Dispatcher.cpp
 *
 *  Created on: Jan 23, 2011
 *      Author: arild
 */
#include <iostream>          // For cout and cerr
#include <cstdlib>           // For atoi()
#include <string.h>
#include <glog/logging.h>
#include <stdio.h>
#include <dlfcn.h>
#include "Config.h"
#include "Dispatcher.h"
#include "ClassLoader.h"
#include <unistd.h>

#define RECV_BUFFER_SIZE	1024
#define TERMINATION_TOKEN	"9797AB"

Dispatcher::Dispatcher(Scheduler *scheduler)
{
	_scheduler = scheduler;
	try {
		_socket = new UDPSocket(MULTICAST_LISTEN_PORT);
	} catch (SocketException &e) {
		LOG(FATAL) << "failed creating udp multicast socket: " << e.what();
	}
	try {
		_socket->joinGroup(MULITCAST_ADDRESS);
	} catch (SocketException &e) {
		LOG(FATAL) << "failed joining multicast group: " << e.what();
	}
}

Dispatcher::~Dispatcher()
{
	delete _socket;
}

void Dispatcher::Start()
{
	LOG(INFO) << "Starting dispatcher...";
	_running = true;
	_thread = boost::thread(&Dispatcher::_ListenForever, this);
	LOG(INFO) << "Dispatcher started";
}

void Dispatcher::Stop()
{
	LOG(INFO) << "Stopping dispatcher...";
	try {
		_running = false;
		// Interrupt blocking dispatcher thread
		_socket->sendTo(TERMINATION_TOKEN, strlen(TERMINATION_TOKEN), "localhost",
				MULTICAST_LISTEN_PORT);
		_thread.join();
	} catch (SocketException &e) {
		LOG(ERROR) << "failed stopping dispatcher: " << e.what();
	}
	LOG(INFO) << "Dispatcher stopped";
}

void Dispatcher::_ListenForever()
{
	char recvBuf[RECV_BUFFER_SIZE];
	string sourceAddress; // Address of datagram source
	unsigned short sourcePort; // Port of datagram source
	IDataCollector *collector = NULL;

	while (_running) {
		try {
			int bytesRcvd = _socket->recvFrom(recvBuf, RECV_BUFFER_SIZE, sourceAddress, sourcePort);
			recvBuf[bytesRcvd] = '\0';

			if (memcmp(recvBuf, TERMINATION_TOKEN, strlen(TERMINATION_TOKEN)) == 0) {
				LOG(INFO) << "termination message received";
				break;
			}
			if (memcmp(recvBuf, PROTOCOL_PREFIX, strlen(PROTOCOL_PREFIX)) != 0) {
				LOG(WARNING) << "invalid message: protocol prefix does not match";
				continue;
			}

			string filePath = strdup(&recvBuf[strlen(PROTOCOL_PREFIX)]);
			LOG(INFO) << "Valid request received: filename=" << filePath << " saddr="
					<< sourceAddress << " | port=" << sourcePort;

			try {
				ClassLoader loader(filePath);
				collector = loader.LoadAndInstantiateCollector();
				LOG(INFO) << "user-defined collector successfully loaded";
			} catch (exception &e) {
				LOG(ERROR) << "failed loading user-defined collector: " << e.what();
			}

			// Adhere to interface contract
			Context *ctx = new Context();
			try {
				collector->OnInit(ctx);
				if (ctx->server.length() == 0)
					// Server not defined, use source
					ctx->server = sourceAddress;

			} catch (exception &e) {
				LOG(ERROR) << "user-defined OnStart() failed: " << e.what();
			}

			_scheduler->RegisterColllector(*collector, ctx);
			LOG(INFO) << "collector successfully registered";
		} catch (SocketException &e) {
			LOG(ERROR) << "socket error: " << e.what();
			break;
		} catch (exception &e) {
			LOG(ERROR) << "dispatcher failed: " << e.what();
			break;
		}
	}
	_socket->disconnect();
}
