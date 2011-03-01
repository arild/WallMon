/*
 * Network.cpp
 *
 *  Created on: Jan 12, 2011
 *      Author: arild
 */

#include <stdio.h>
#include <iostream>           // For cerr and cout
#include <cstdlib>            // For atoi()
#include <netinet/in.h>
#include <unistd.h>
#include <glog/logging.h>
#include "DataSink.h"
#include "Config.h"
#include "ByteBuffer.h"

#define NUM_BYTES_LOG_INTERVAL	(1024 * 1000); // 1MB
#define RECEIVE_BUF_START_SIZE	1024 * 100 // 100 KB
using namespace std;


void printb(char *s, int len)
{
	int i;
	for (i = 0; i < len; i++)
		LOG(INFO) << "Byte at index " << i << ": " << (int) s[i];
}

// Declared static variables. Static because there are some scope
// problems with libev during the socket read callback
DataRouter *DataSink::_router;
unsigned int DataSink::_numConnectedClients;
unsigned int DataSink::_totalNumBytesReceived;
unsigned int DataSink::_numBytesLogTrigger;

DataSink::DataSink(DataRouter *router)
{
	struct sockaddr_in addr;
	int sd;

	// Static variables
	_router = router;
	_numConnectedClients = 0;
	_totalNumBytesReceived = 0;
	_numBytesLogTrigger = NUM_BYTES_LOG_INTERVAL;

	// Socket setup
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(STREAMER_ENTRY_PORT);
	addr.sin_addr.s_addr = INADDR_ANY;

	sd = socket(PF_INET, SOCK_STREAM, 0);
	LOG_IF(FATAL, sd < 0) << "failed creating socket";
	int ret = ::bind(sd, (struct sockaddr *) &addr, sizeof(addr));
	LOG_IF(FATAL, ret != 0) << "failed binding socket";
	ret = listen(sd, 2);
	LOG_IF(FATAL, ret < 0) << "failed listening on socket";

	// Libev setup - dispatch accept watcher
	_loop = ev_default_loop(0);
	ev::io *accept_watcher = new ev::io(_loop);
	accept_watcher->set<DataSink, &DataSink::_AcceptCallback> (this);
	accept_watcher->start(sd, 1);

	// Book-keeping for cleanup
	_watcherSet = new boost::unordered_set<ev::io *>();
	_watcherSet->insert(accept_watcher);
}

DataSink::~DataSink()
{
	ev_loop_destroy(_loop);
}

void DataSink::Start()
{
	_running = true;
	_thread = boost::thread(&DataSink::_ServeForever, this);
}

void DataSink::Stop()
{
	boost::unordered_set<ev::io *>::iterator iter;
	for (iter = _watcherSet->begin(); iter != _watcherSet->end(); iter++)
		_DeleteWatcher(*(*iter));
	//_thread.join(); // Can take up to several seconds
	LOG(INFO) << "DataSink stopped";
}

void DataSink::_ServeForever()
{
	ev_run(_loop, 0);
}

/* Accept client requests */
void DataSink::_AcceptCallback(ev::io &watcher, int revents)
{
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client_sd;

	if (EV_ERROR & revents) {
		LOG(WARNING) << "got invalid event";
		return;
	}

	client_sd = accept(watcher.fd, (struct sockaddr *) &client_addr, &client_len);
	if (client_sd < 0) {
		LOG(WARNING) << "socket accept() failed";
		return;
	}
	LOG(INFO) << ++_numConnectedClients << " client(s) connected";

	// Initialize and start watcher to read client requests
	ev::io *readWatcher = new ev::io(watcher.loop);
	readWatcher->set<DataSink, &DataSink::_ReadCallback> (this);
	readWatcher->data = new ByteBuffer(RECEIVE_BUF_START_SIZE); // Buffer for saving concatenated messages
	readWatcher->start(client_sd, 1);
	_watcherSet->insert(readWatcher);
}

void DataSink::_ReadCallback(ev::io &watcher, int revents)
{
	unsigned int numBytesReceived;

	if (EV_ERROR & revents) {
		LOG(WARNING) << "got invalid event";
		return;
	}
	ByteBuffer *buf = (ByteBuffer *) watcher.data;
	buf->Defragment();
	int len = buf->GetContinuousBytesLeft();
	numBytesReceived = read(watcher.fd, buf->GetWriteReference(), len);
	buf->BytesWritten(numBytesReceived);
	//LOG(INFO) << "Received: " << numBytesReceived;
	if (numBytesReceived < 0) {
		LOG(ERROR) << "failed reading socket - client probably disconnected";
		return;
	}

	if (numBytesReceived == 0) {
		// Stop and free watcher if client socket is closing
		_DeleteWatcher(watcher);
		LOG(INFO) << --_numConnectedClients << " client(s) connected";
		return;
	}

	int frag = 0;
	while (buf->GetBytesAvailable() > 4) {
		// As long as there are more than 4 bytes available there could be
		// (at least) one more full message in the buffer
		int messageLength = ntohl(*((unsigned int *) buf->GetReadReference()));
		//LOG(INFO) << "MSG length: " << messageLength;
		if (messageLength > buf->GetCapacity() - 4) {
			// Current buffer too small, double the capacity at a minimum
			// and wait until next read on socket
			if (messageLength > buf->GetCapacity() * 2)
				buf->Resize(messageLength);
			else
				buf->Resize(buf->GetCapacity() * 2);

			LOG(INFO) << "Buffer resized, new capacity: " << buf->GetCapacity();
			break;
		}
		if (buf->GetBytesAvailable() - 4 < messageLength)
			// Fragmented packet, wait until next event.
			// Note that the same message length header will be read over again
			break;
		// There is still at least one more full message available
		//LOG(INFO) << "Routing message of size: " << messageLength;
		buf->BytesRead(4);
		//printb(buf->GetReadReference(), 10);
		_router->Route(buf->GetReadReference(), messageLength);
		buf->BytesRead(messageLength);
		frag++;
		if (frag >= 2)
			LOG(INFO) << "Fragmentation: " << frag << " | Bytes received: " << numBytesReceived;
	}

	_totalNumBytesReceived += numBytesReceived;
	if (_totalNumBytesReceived >= _numBytesLogTrigger) {
		int megaBytesStreamed = _totalNumBytesReceived / NUM_BYTES_LOG_INTERVAL;
		LOG(ERROR) << megaBytesStreamed << " MB received";
		_numBytesLogTrigger += NUM_BYTES_LOG_INTERVAL;
	}
}

void DataSink::_DeleteWatcher(ev::io &watcher)
{
	LOG(INFO) << "Stopping watcher " << watcher.fd;
	watcher.stop(); // Make watcher inactive
	close(watcher.fd); // Close associated socket
	delete &watcher; // Release memory
	_watcherSet->erase(&watcher);
}

