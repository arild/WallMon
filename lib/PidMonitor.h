/*
 * PidMonitor.h
 *
 *  Created on: Apr 16, 2011
 *      Author: arild
 */

#ifndef PIDMONITOR_H_
#define PIDMONITOR_H_

#include <vector>
#include <set>
#include "System.h"

using namespace std;


class PidMonitor {
public:
	PidMonitor();
	virtual ~PidMonitor();
	void Update();
	vector<int> GetDifference();

private:
	vector<int> *_new, *_old;
};

#endif /* PIDMONITOR_H_ */
