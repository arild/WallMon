/*
 * Network.h
 *
 *  Created on: Jan 12, 2011
 *      Author: arild
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <string>
#include <map>
#include <boost/thread.hpp>
#include "Queue.h"

using namespace std;
typedef map<string, int> ServerMap;

class StreamItem;

class Streamer {
public:
	Streamer();
	virtual ~Streamer();
	void Start();
	void Stop();
	int SetupStream(string serverAddress);
	StreamItem &StreamItemFactory(void **dataItems, int *lengthItems, int numItems, int sockfd);
	void Stream(StreamItem &item);

private:
	void _StreamForever();
	boost::thread _thread;
	bool _running;
	Queue<StreamItem *> *_queue;
	ServerMap *_serverMap;
	int _numBytesStreamed;
	int _numBytesLogTrigger;
};

#endif /* NETWORK_H_ */
