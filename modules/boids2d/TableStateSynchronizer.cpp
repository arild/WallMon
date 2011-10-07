/*
 * TableStateManager.cpp
 *
 *  Created on: Oct 7, 2011
 *      Author: arild
 */
#include <glog/logging.h>
#include "Config.h"
#include "TableStateSynchronizer.h"

#define RECV_BUFFER_SIZE	1024 * 10
#define MESSAGE_BUF_SIZE	1024 * 50

TableStateSynchronizer::TableStateSynchronizer()
{
	try {
		_socket = new UDPSocket(STATE_MULTICAST_LISTEN_PORT);
	} catch (SocketException &e) {
		LOG(FATAL) << "failed creating udp multicast socket: " << e.what();
	}
	try {
		_socket->joinGroup(MULITCAST_ADDRESS);
	} catch (SocketException &e) {
		LOG(FATAL) << "failed joining multicast group: " << e.what();
	}
}

TableStateSynchronizer::~TableStateSynchronizer()
{
}

void TableStateSynchronizer::Start()
{
	_running = true;
	_thread = boost::thread(&TableStateSynchronizer::_ListenForever, this);
}

void TableStateSynchronizer::Stop()
{
	LOG(INFO) << "stopping state synchronizer...";
	_running = false;
	_socket->sendTo("", 0, "localhost", STATE_MULTICAST_LISTEN_PORT);
	_thread.join();
	LOG(INFO) << "state synchronizer stopped";
}

Queue<TableStateMessage> *TableStateSynchronizer::GetStateMessageQueue(bool isTopTable)
{
	if (isTopTable)
		return &_topTableMessageQueue;
	return &_subTableMessageQueue;
}

void TableStateSynchronizer::SynchronizeState(TableStateMessage &msg)
{
	char buf[MESSAGE_BUF_SIZE];
	if (msg.SerializeToArray(buf, MESSAGE_BUF_SIZE) != true) {
		LOG(ERROR) << "Protocol buffer serialization failed";
		return;
	}
	_socket->sendTo(buf, msg.ByteSize(), MULITCAST_ADDRESS, STATE_MULTICAST_LISTEN_PORT);
	LOG(INFO) << "state message sent";
}

void TableStateSynchronizer::_ListenForever()
{
	char recvBuf[RECV_BUFFER_SIZE];
	string sourceAddress; // Address of datagram source
	unsigned short sourcePort; // Port of datagram source
	TableStateMessage msg;
	LOG(INFO) << "state synchronizer started and serving requests";
	while (true) {
		try {
			int bytesRcvd = _socket->recvFrom(recvBuf, RECV_BUFFER_SIZE, sourceAddress, sourcePort);
			if (_running == false)
				break;
			if (msg.ParseFromArray(recvBuf, bytesRcvd) == false) {
				LOG(ERROR) << "failed parsing state message";
				continue;
			}
		} catch (exception &e) {
			LOG(ERROR) << "state synchronizer failed: " << e.what();
			break;
		}
		LOG(INFO) << "state message received, forwarding it";
		if (msg.istoptable())
			_topTableMessageQueue.Push(msg);
		else
			_subTableMessageQueue.Push(msg);
	}
}

