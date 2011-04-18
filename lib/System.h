/*
 * System.h
 *
 *  Created on: Feb 13, 2011
 *      Author: arild
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <string>
#include <vector>

using namespace std;

class System {
public:
	static void Daemonize();
	static int GetPid(string processName);
	static vector<int> *GetAllPids();
	static double GetTimeInSec();
	static double GetTimeInMsec();
	static string &GetHostname();
	static int GetNumCores();
	static int GetTotalMemory();
	static bool HasSupportForProcPidIo();
};

#endif /* SYSTEM_H_ */
