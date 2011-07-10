/**
 * @file   DataSink.cpp
 * @Author Arild Nilsen
 * @date   January, 2011
 *
 * Manages arbitrary many client TCP connections and forwards
 * received data to the DataRouter.
 */

#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h> // inet_ntoa()
#include <boost/foreach.hpp>
#include <glog/logging.h>
#include "DataSink.h"
#include "Config.h"
#include "ByteBuffer.h"

#define RECEIVE_BUF_START_SIZE	1024 * 100 // 100 KB
// Declared static variables. Static because there are some scope
// problems with libev during the socket read callback

WatcherMap DataSink::_watcherMap;
DataRouter *DataSink::_router;
IoLogger *DataSink::_ioLogger;
unsigned int DataSink::_numConnectedClients;

DataSink::DataSink(DataRouter *router)
{
	// Static variables
	_router = router;
	_ioLogger = new IoLogger(1024 * 1000 * 10);
	_numConnectedClients = 0;

	// Setup the entry socket which does the accept() call
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(STREAMER_ENTRY_PORT);
	addr.sin_addr.s_addr = INADDR_ANY;

	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	LOG_IF(FATAL, sockfd < 0) << "failed creating socket";

	unsigned int reuse = 1;
	int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	LOG_IF(FATAL, ret < 0) << "failed setting so_reuseaddr";

	ret = ::bind(sockfd, (struct sockaddr *) &addr, sizeof(addr));
	LOG_IF(FATAL, ret != 0) << "failed binding socket";

	ret = listen(sockfd, 2);
	LOG_IF(FATAL, ret < 0) << "failed listening on socket";

	// libev setup: bind a watcher to the accept socket file descriptor
	_loop = ev_default_loop(0);
	ev::io *accept_watcher = new ev::io(_loop);
	accept_watcher->set<DataSink, &DataSink::_AcceptCallback> (this);
	accept_watcher->data = NULL;
	accept_watcher->start(sockfd, EV_READ);

	// Book-keeping for cleanup
	_watcherMap[accept_watcher] = "";
}

DataSink::~DataSink()
{
	ev_loop_destroy(_loop);
}

void DataSink::Start()
{
	_thread = boost::thread(&DataSink::_ServeForever, this);
}

void DataSink::Stop()
{
	LOG(INFO) << "stopping DataSink...";
	BOOST_FOREACH (WatcherMap::value_type &item, _watcherMap)
		_DeleteAndCleanupWatcher(*item.first);
	ev_break(_loop, EVBREAK_ALL);
	//_thread.join(); // Can take up to several seconds
	LOG(INFO) << "DataSink stopped";
}

void DataSink::_ServeForever()
{
	LOG(INFO) << "DataSink started and serving requests";
	ev_run(_loop, 0);
}

/* Accept client requests */
void DataSink::_AcceptCallback(ev::io &watcher, int revents)
{
	if (EV_ERROR & revents) {
		LOG(WARNING) << "invalid event";
		return;
	}

	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int clientSockfd = accept(watcher.fd, (struct sockaddr *) &client_addr, &client_len);
	if (clientSockfd < 0) {
		LOG(WARNING) << "socket accept() failed";
		return;
	}
	string clientIpAddress = (string)inet_ntoa(client_addr.sin_addr);
	LOG(INFO) << "client connected: " << clientIpAddress <<  " -> "
			<< ++_numConnectedClients << " client(s) connected";

	// Initialize and start watcher to read client requests
	ev::io *clientWatcher = new ev::io(watcher.loop);
	clientWatcher->set<DataSink, &DataSink::_ReadCallback> (this);
	clientWatcher->data = new ByteBuffer(RECEIVE_BUF_START_SIZE); // Buffer for saving concatenated messages
	clientWatcher->start(clientSockfd, EV_READ);
	_watcherMap[clientWatcher] = clientIpAddress;
}

void DataSink::_ReadCallback(ev::io &watcher, int revents)
{
	if (EV_ERROR & revents) {
		LOG(WARNING) << "got invalid event";
		return;
	}
	ByteBuffer *buf = (ByteBuffer *) watcher.data;
	buf->Defragment();
	int len = buf->GetContinuousBytesLeft();
	unsigned int numBytesReceived = read(watcher.fd, buf->GetWriteReference(), len);
	buf->BytesWritten(numBytesReceived);
	if (numBytesReceived < 0) {
		LOG(ERROR) << "failed reading socket - client probably disconnected";
		return;
	}

	if (numBytesReceived <= 0) {
		// Stop and free watcher if client socket is closing
		LOG(INFO) << "client disconnected: " << _watcherMap[&watcher] << " -> "
				<< --_numConnectedClients << " client(s) connected";
		_DeleteAndCleanupWatcher(watcher);
		return;
	}

	while (buf->GetBytesAvailable() > 4) {
		// As long as there are more than 4 bytes available there could be
		// (at least) one more full message in the buffer
		int messageLength = ntohl(*((unsigned int *) buf->GetReadReference()));
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
		buf->BytesRead(4);
		_router->Route(buf->GetReadReference(), messageLength);
		buf->BytesRead(messageLength);
	}

	_ioLogger->Read(numBytesReceived);
}

void DataSink::_DeleteAndCleanupWatcher(ev::io &watcher)
{
	watcher.stop(); // Make watcher inactive
	close(watcher.fd); // Close associated socket
	int numWatchersRemoved = _watcherMap.erase(&watcher);
	CHECK(numWatchersRemoved == 1);
	delete &watcher; // Release memory
}

