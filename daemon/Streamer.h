/*
 * Network.h
 *
 *  Created on: Jan 12, 2011
 *      Author: arild
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <boost/thread.hpp>
#include "Queue.h"
#include <string>
#include <map>

typedef std::map<std::string, int> ServerMap;
using namespace std;

typedef struct StreamItem {
	void *data;
	int length;
	int sockfd;
} StreamItem_t;

class Streamer {
public:
	Streamer();
	virtual ~Streamer();
	void Start();
	void Stop();
	int RegisterServerAddress(string serverAddress);
	void Stream(StreamItem &item);

private:
	void _StreamForever();
	boost::thread _thread;
	bool _running;
	Queue<StreamItem> *_queue;
	ServerMap _serverMap;
	int _numBytesStreamed;
	int _numBytesLogTrigger;
};

#endif /* NETWORK_H_ */
