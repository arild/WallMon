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
#include "Config.h"
#include "Streamer.h"

#define NUM_BYTES_LOG_INTERVAL	1 << 20; // 1MB

Streamer::Streamer()
{
	_numBytesStreamed = 0;
	_numBytesLogTrigger = NUM_BYTES_LOG_INTERVAL;
	_queue = new Queue<StreamItem *> (100);
	_serverMap = new ServerMap();
}

Streamer::~Streamer()
{
	delete _queue;
	delete _serverMap;
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
 * @return File descriptor of socket connected to specified server address
 */
int Streamer::SetupStream(string serverAddress)
{
	// Check if address already is associated with a file descriptor
	ServerMap::iterator it;
	for (it = _serverMap->begin(); it != _serverMap->end(); it++)
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
	LOG(INFO) << "Streamer successfully connected to server";

	// Save the relationship: server_address -> socket_fd
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

/**
 * Removes packet from FIFO queue and sends them on an open socket,
 * which should have been created and set up by Streamer::Connect().
 */
void Streamer::_StreamForever()
{
	LOG(INFO) << "Streamer started and serving requests";
	while (true) {
		StreamItem *item = _queue->Pop();
		if (_running == false)
			// Does not have to release memory of termination item
			break;

		int numBytesSent = 0;
		do {
			numBytesSent += write(item->sockfd, item->message, item->messageLength);
			if (numBytesSent == -1) {
				LOG(ERROR) << "stream socket down";
				break;
			}
		} while (numBytesSent < item->messageLength);
		LOG_IF(FATAL, numBytesSent != item->messageLength)<< "all bytes not sent";

		// Release the memory originally allocated in StreamItemFactory()
		//delete item->packet;
		delete item;

		_numBytesStreamed += numBytesSent;
		if (_numBytesStreamed >= _numBytesLogTrigger) {
			int megaBytesStreamed = _numBytesStreamed / NUM_BYTES_LOG_INTERVAL;
			LOG(INFO) << megaBytesStreamed << " bytes streamed";
			_numBytesLogTrigger += NUM_BYTES_LOG_INTERVAL;
		}
	}

	// Close all sockets that have been opened
	BOOST_FOREACH (ServerMap::value_type &i, *_serverMap)
	{
		close(i.second);
	}
}






