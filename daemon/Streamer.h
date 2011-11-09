#ifndef STREAMER_H_
#define STREAMER_H_

#include <string>
#include <map>
#include <boost/thread.hpp>
#include "Queue.h"
#include "StreamItem.h"
#include "IoLogger.h"

using namespace std;
typedef map<string, int> ServerMap;

class Streamer {
public:
	Streamer();
	virtual ~Streamer();
	void Start();
	void Stop();
	int SetupStream(string serverAddress, int serverPort);
	void Stream(StreamItem &item);
	unsigned int GetNumPendingItems();
	unsigned int GetNumServerConnections();

private:
	boost::thread _thread;
	bool _running;
	Queue<StreamItem *> *_queue;
	ServerMap *_serverMap;
	IoLogger *_ioLogger;
	void _StreamForever();
	void _SendItem(StreamItem &item);
	int _SendAll(int sockFd, char *message, int len);
	void _RemoveAndCleanupSockFd(int sockFd);
};

#endif /* STREAMER_H_ */
