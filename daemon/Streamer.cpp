/**
 * @file   Network.cpp
 * @Author Arild Nilsen
 * @date   January, 2011
 *
 * Removes user-defined data from a FIFO queue and sends it to a
 * specified, possibly, remote location.
 */

#include <glog/logging.h>
#include <sys/socket.h>
#include <arpa/inet.h> // inet_addr()
#include <boost/foreach.hpp>
#include <netinet/tcp.h> // TCP_NODELAY
#include <stdio.h> // perror
#include "Config.h"
#include "Streamer.h"
#include "System.h"

Streamer::Streamer()
{
	_queue = new Queue<StreamItem *> (100);
	_serverMap = new ServerMap();
	_ioLogger = new IoLogger(1024 * 1000 * 10);
}

Streamer::~Streamer()
{
	delete _queue;
	delete _serverMap;
	delete _ioLogger;
}

void Streamer::Start()
{
	_running = true;
	_thread = boost::thread(&Streamer::_StreamForever, this);
}

/**
 * Stops the streamer thread by changing the thread's termination flag and
 * then putting a dummy item on the queue that the thread listen on.
 */
void Streamer::Stop()
{
	LOG(INFO) << "stopping Streamer...";
	_running = false;
	StreamItem item(1);
	_queue->Push(&item);
	_thread.join();
	LOG(INFO) << "Streamer stopped";
}

/**
 * Returns a socket descriptor for the given server address.
 *
 * Unless a descriptor already is available for the given address, the
 * method will attempt to set up a new TCP connection to the address.
 * This method can be called arbitrary many times, however, there is only
 * one streamer thread. Users are responsible for supplying the streamer
 * thread with the file descriptor they wish the streamer thread to write to.
 *
 * @param serverAddress  Either an IP address or hostname
 * @return File descriptor of socket connected to specified server address
 */
int Streamer::SetupStream(string serverAddress, int serverPort)
{
	// Check if address already is associated with a file descriptor
	if (_serverMap->count(serverAddress) > 0)
		return (*_serverMap)[serverAddress];

	LOG(INFO) << "new server: " << serverAddress << ":" << serverPort;
	string serverIpAddress = "";
	if (System::IsValidIpAddress(serverAddress) == false) {
		// Translate hostname to an ip address
		serverIpAddress = System::HostnameToIpAddress(serverAddress);
		if (serverIpAddress.empty()) {
			LOG(ERROR) << "unable to resolve hostname: " << serverAddress << ", trying fallback approach...";
			serverIpAddress = System::HostnameToIpAddressFallback(serverAddress);
			if (serverIpAddress.empty()) {
				LOG(ERROR) << "fallback approach failed too";
				return -1;
			}
		}
		if (System::IsValidIpAddress(serverIpAddress) == false) {
			LOG(ERROR) << "resolved hostname to invalid ip address: " << serverIpAddress;
			return -1;
		}
	}

	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(serverIpAddress.c_str());
	addr.sin_port = htons(serverPort);

	int fd = socket(PF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		perror("failed creating socket: ");
		LOG(FATAL);
	}

	// Disable the Nagle (TCP No Delay) algorithm
	int flag = 1;
	int ret = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));
	LOG_IF(FATAL, ret == -1) << "setting TCP_NODELAY failed";

	ret = connect(fd, (struct sockaddr *) &addr, sizeof(addr));
	if (ret < 0) {
		LOG(ERROR) << "failed connecting to server";
		return -1;
	}
	LOG(INFO) << "Streamer successfully connected to server: " << serverAddress;

	// Book-keep the relationship: server_address -> socket_fd
	(*_serverMap)[serverAddress] = fd;
	return fd;
}

/**
 * Pushes an item on the FIFO queue that is continuously
 * being drained by the streamer thread
 */
void Streamer::Stream(StreamItem &item)
{
	_queue->Push(&item);
}

unsigned int Streamer::GetNumPendingItems()
{
	return _queue->GetSize();
}

unsigned int Streamer::GetNumServerConnections()
{
	return _serverMap->size();
}

/**
 * Removes packet from FIFO queue and sends them on an open socket,
 * which should have been created and set up by Streamer::Connect().
 */
void Streamer::_StreamForever()
{
	LOG(INFO) << "Streamer started and serving requests";
	int maxQueueSize = -1;
	while (true) {
		StreamItem *item = _queue->Pop();
		if (_running == false)
			// Does not have to release memory of termination item
			break;

		if (_queue->GetSize() > maxQueueSize) {
			maxQueueSize = _queue->GetSize();
			LOG(INFO) << "max queue size: " << maxQueueSize;
		}

		_SendItem(*item);
		// Release the memory originally allocated in StreamItem()
		delete item;
	}

	// Close all sockets that have been opened
	BOOST_FOREACH (ServerMap::value_type &i, *_serverMap)
	{
		close(i.second);
	}
}

void Streamer::_SendItem(StreamItem & item)
{
	BOOST_FOREACH (int sockFd, item.serversSockFd)
	{
		int numBytesSent = _SendAll(sockFd, item.message, item.messageLength);
		_ioLogger->Write(numBytesSent);
		if (numBytesSent != item.messageLength) {
			LOG(ERROR) << "all bytes not sent, connection probably broken";
			_RemoveAndCleanupSockFd(sockFd);
		}
	}
}

int Streamer::_SendAll(int sockFd, char *message, int len)
{
	int numBytesSent = 0;
	do {
		int retval = send(sockFd, message + numBytesSent, len - numBytesSent, MSG_NOSIGNAL);
		if (retval == -1)
			return numBytesSent;
		numBytesSent += retval;
	} while (numBytesSent < len);
	return numBytesSent;
}

void Streamer::_RemoveAndCleanupSockFd(int sockFd)
{
	BOOST_FOREACH (ServerMap::value_type &i, *_serverMap)
	{
		string serverAddress = i.first;
		int _sockFd = i.second;
		if (sockFd == _sockFd) {
			_serverMap->erase(serverAddress);
			close(sockFd);
			break;
		}
	}
}



