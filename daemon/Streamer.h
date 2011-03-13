#ifndef STREAMER_H_
#define STREAMER_H_

#include <string>
#include <map>
#include <boost/thread.hpp>
#include "Queue.h"
#include "StreamItem.h"

using namespace std;
typedef map<string, int> ServerMap;

class Streamer {
public:
	Streamer();
	virtual ~Streamer();
	void Start();
	void Stop();
	int SetupStream(string serverAddress);
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

#endif /* STREAMER_H_ */
