/*
 * PidMonitor.h
 *
 *  Created on: Apr 16, 2011
 *      Author: arild
 */

#ifndef PIDMONITOR_H_
#define PIDMONITOR_H_

#include <set>
#include "System.h"

using namespace std;


class PidMonitor {
public:
	PidMonitor();
	virtual ~PidMonitor();
	void Update();
	set<int> GetDiff();
	void Ignore(int pid);

private:
	set<int> *_pids;
};

#endif /* PIDMONITOR_H_ */
