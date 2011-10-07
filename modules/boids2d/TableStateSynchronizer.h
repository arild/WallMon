/*
 * TableStateManager.h
 *
 *  Created on: Oct 7, 2011
 *      Author: arild
 */

#ifndef TABLESTATESYNCHRONIZER_H_
#define TABLESTATESYNCHRONIZER_H_

#include <boost/thread.hpp>
#include <map>
#include "Queue.h"
#include "PracticalSocket.h" // For UDPSocket and SocketException
#include "stubs/TableState.pb.h"
using namespace std;


class TableStateSynchronizer {
public:
	TableStateSynchronizer();
	virtual ~TableStateSynchronizer();
	void Start();
	void Stop();
	Queue<TableStateMessage> *GetStateMessageQueue(bool isTopTable);
	void SynchronizeState(TableStateMessage &msg);
private:
	Queue<TableStateMessage> _topTableMessageQueue, _subTableMessageQueue;
	boost::thread _thread;
	bool _running;
	UDPSocket *_socket;
	void _ListenForever();
};

#endif /* TABLESTATESYNCHRONIZER_H_ */
