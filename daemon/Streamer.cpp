/*
 * Network.cpp
 *
 *  Created on: Jan 12, 2011
 *      Author: arild
 */

#include <stdio.h>
#include <iostream>           // For cerr and cout
#include <cstdlib>            // For atoi()
#include <glog/logging.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>

#include "Config.h"
#include "Streamer.h"

#define NUM_BYTES_LOG_INTERVAL	1 << 20; // 1MB
void printb(char *s, int len)
{
	int i;
	for (i = 0; i < len; i++)
		LOG(INFO) << "Byte at index " << i << ":" << (int) s[i];
}

Streamer::Streamer()
{
	_numBytesStreamed = 0;
	_numBytesLogTrigger = NUM_BYTES_LOG_INTERVAL;
	_queue = new Queue<StreamItem> (100);
	_serverMap = *new ServerMap();
}

Streamer::~Streamer()
{
	delete _queue;
}

// TODO: Deal with possible concurrency problems
int Streamer::RegisterServerAddress(string serverAddress)
{
	// Check if address already is associated with a file descriptor
	ServerMap::iterator it;
	for (it = _serverMap.begin(); it != _serverMap.end(); it++)
		if ((*it).first == serverAddress)
			return (*it).second;

	struct sockaddr_in addr;

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(serverAddress.c_str());
	addr.sin_port = htons(STREAMER_ENTRY_PORT);

	int fd = socket(PF_INET, SOCK_STREAM, 0);
	LOG_IF(FATAL, fd < 0)<< "failed creating socket";
	int ret = connect(fd, (struct sockaddr *) &addr, sizeof(addr));
	LOG_IF(FATAL, ret < 0) << "failed connecting to server";
	LOG(INFO) << "streamer successfully connected to server";

	// Save the relationship: server_address -> socket_fd
	_serverMap[serverAddress] = fd;
	return fd;
}

void Streamer::Start()
{
	LOG(INFO) << "Starting streamer...";
	_running = true;
	_thread = boost::thread(&Streamer::_StreamForever, this);
	LOG(INFO) << "Streamer started";
}

/**
 * Stops the streamer thread by changing the thread's termination flag and
 * then putting a dummy item on the queue that the thread listen on.
 */
void Streamer::Stop()
{
	_running = false;
	StreamItem_t item;
	_queue->Push(item);
	_thread.join();
	LOG(INFO) << "Streamer terminated";
}

void Streamer::Stream(StreamItem &item)
{
	_queue->Push(item);
}

void Streamer::_StreamForever()
{
	while (true) {
		LOG(INFO) << "Streamer popping item...";
		StreamItem item = _queue->Pop();
		LOG(INFO) << "Item popped";
		if (_running == false)
			break;

		int headerLength = sizeof(unsigned int);
		int packetLength = headerLength + item.length;
		char packet[packetLength];
		int dataLengthNetworkByteOrder = htonl(item.length);
		memcpy(packet, &dataLengthNetworkByteOrder, sizeof(int));
		memcpy(packet + headerLength, item.data, item.length);

		//printb((char *)&packet[4], 10);
		int numBytesSent = write(item.sockfd, packet, packetLength);//, MSG_CONFIRM);
		LOG(INFO) << "bytes sent: " << packetLength;
		if (numBytesSent == -1) {
			LOG(ERROR) << "stream socket down";
			break;
		}
		LOG_IF(FATAL, numBytesSent != packetLength)<< "all bytes not sent";

		_numBytesStreamed += numBytesSent;
		if (_numBytesStreamed >= _numBytesLogTrigger) {
			int megaBytesStreamed = _numBytesStreamed / NUM_BYTES_LOG_INTERVAL;
			LOG(INFO) << megaBytesStreamed << " bytes streamed";
			_numBytesLogTrigger += NUM_BYTES_LOG_INTERVAL;
		}
	}
//	close(_sockfd);
}
