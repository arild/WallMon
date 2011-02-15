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
#include "DataSink.h"
#include "Config.h"
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

DataSink::DataSink(DataRouter *router)
{
	struct sockaddr_in addr;
	int sd;

	_router = router;
	_numConnectedClients = 0;

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(STREAMER_ENTRY_PORT);
	addr.sin_addr.s_addr = INADDR_ANY;

	if ((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		LOG(ERROR) << "failed creating socket";
	if (::bind(sd, (struct sockaddr *) &addr, sizeof(addr)) != 0)
		LOG(ERROR) << "failed binding socket";
	if (listen(sd, 2) < 0)
		LOG(ERROR) << "failed listening on socket";

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
	LOG(INFO) << "JOIN";
	//_thread.join(); // Can take up to several seconds
	LOG(INFO) << "JOIN DONE";
}

void DataSink::_ServeForever()
{
	LOG(INFO) << "DataSink started";
	ev_run(_loop, 0);
	LOG(INFO) << "DataSink stopped";
}

/* Accept client requests */
void DataSink::_AcceptCallback(ev::io &watcher, int revents)
{
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client_sd;

	if (EV_ERROR & revents) {
		LOG(ERROR) << "got invalid event";
		return;
	}

	client_sd = accept(watcher.fd, (struct sockaddr *) &client_addr, &client_len);
	if (client_sd < 0) {
		LOG(ERROR) << "socket accept() error";
		return;
	}
	_numConnectedClients += 1;
	LOG(INFO) << _numConnectedClients << " client(s) connected";

	// Initialize and start watcher to read client requests
	ev::io *readWatcher = new ev::io(watcher.loop);
	readWatcher->set<DataSink, &DataSink::_ReadCallback> (this);
	readWatcher->start(client_sd, 1);
	_watcherSet->insert(readWatcher);
}

void DataSink::_ReadCallback(ev::io &watcher, int revents)
{
	char buffer[1024];
	unsigned int numBytesRead;

	if (EV_ERROR & revents) {
		perror("got invalid event");
		return;
	}

	int clientSocketDescriptor = watcher.fd;
	numBytesRead = recv(clientSocketDescriptor, buffer, 1024, 0);
	if (numBytesRead < 0) {
		LOG(ERROR) << "failed reading socket";
		return;
	}

	if (numBytesRead == 0) {
		// Stop and free watcher if client socket is closing
		_DeleteWatcher(watcher);
		_numConnectedClients -= 1;
		LOG(INFO) << _numConnectedClients << " client(s) connected";
		return;
	}
	unsigned int *messageLength = (unsigned int *)buffer;
	if (*messageLength != numBytesRead - 4)
		// TODO: This can happen, just find out when
		LOG(ERROR) << "TCP packet not read in one operation (FIX!)";
	_router->Route(&buffer[4], *messageLength);
}

void DataSink::_DeleteWatcher(ev::io &watcher)
{
	LOG(INFO) << "Stopping watcher " << watcher.fd;
	watcher.stop(); // Make watcher inactive
	close(watcher.fd); // Close associated socket
	delete &watcher; // Release memory
	_watcherSet->erase(&watcher);
}

