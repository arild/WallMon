/*
 * System.h
 *
 *  Created on: Feb 13, 2011
 *      Author: arild
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <string>
#include <list>


using std::string;
using std::list;

class System {
public:
	static void Daemonize();
	static int GetPid(string processName);
	static list<int> *GetAllPids();
	static double GetTimeInSec();
};

#endif /* SYSTEM_H_ */
