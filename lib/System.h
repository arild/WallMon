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
	static int GetNumLogicalCores();
	static int GetNumRealCores();
	static int GetTotalMemory();
	static bool HasSupportForProcPidIo();
	static bool IsRocksvvCluster();
	static bool IsValidIpAddress(string ipAddress);
	static vector<string> HostnameToIpAddress(string hostname);
	static void ExportDisplayToLocalhost();
	static string GetCurrentUser();
	static bool IsRocksvvClusterRootNode();
	static bool IsIceClusterRootNode();
	static string RunCommand(string cmd);
	static string RunCommand(char *cmd);
};

#endif /* SYSTEM_H_ */
