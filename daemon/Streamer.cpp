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

#include "Config.h"
#include "Streamer.h"
#include "PracticalSocket.h"
using namespace std;


#define NUM_BYTES_LOG_INTERVAL	1 << 20; // 1MB


Streamer::Streamer(string serverAddress)
{
	_numBytesStreamed = 0;
	_numBytesLogTrigger = NUM_BYTES_LOG_INTERVAL;
	try {
		_socket = new TCPSocket(serverAddress, STREAMER_ENTRY_PORT);
		LOG(INFO) << "streamer successfully connected to server";
	} catch (SocketException &e) {
		LOG(ERROR) << "failed connecting to server: " << e.what();
		exit(1);
	}
}

Streamer::~Streamer()
{
	delete _socket;
}

int Streamer::Stream(string key, void *data, int dataLength)
{
	int messageLengthHeader = key.length() + dataLength;
	int totalLength = messageLengthHeader + 4;
	LOG(INFO) << "Sending num bytes: " << messageLengthHeader;
	char packet[totalLength];

	unsigned int tmp = htonl(messageLengthHeader);
	memcpy(packet, &tmp, sizeof(int));
	memcpy(packet + sizeof(int), key.c_str(), key.length() + 1);
	memcpy(packet + sizeof(int) + key.length() + 1, data, dataLength);

	_socket->send(packet, totalLength);
	_numBytesStreamed += totalLength;
	if (_numBytesStreamed >= _numBytesLogTrigger) {
		int megaBytesStreamed = _numBytesStreamed/NUM_BYTES_LOG_INTERVAL;
		LOG(INFO) << megaBytesStreamed << " bytes streamed";
		_numBytesLogTrigger += NUM_BYTES_LOG_INTERVAL;
	}
	return 0;
}

void Streamer::SetStreamInterval()
{

}
