/*
 * Network.h
 *
 *  Created on: Jan 12, 2011
 *      Author: arild
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include "PracticalSocket.h"


class Streamer {
public:
	Streamer(string serverAddress);
	virtual ~Streamer();
	int Stream(string key, void *data, int length);
	void SetStreamInterval();
private:
	TCPSocket *_socket;
	int _numBytesStreamed;
	int _numBytesLogTrigger;
};

#endif /* NETWORK_H_ */
