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
	static vector<int> GetAllPids();
	static bool GetAllPids(vector<int> &pids);

	static double GetTimeInSec();
	static double GetTimeInMsec();
	static string &GetHostname();
	static int GetNumLogicalCores();
	static int GetNumRealCores();
	static int GetTotalMemory();
	static bool IsValidIpAddress(string ipAddress);
	static string HostnameToIpAddress(string hostname);
	static string HostnameToIpAddressFallback(string hostname);
	static void AttachToLocalDisplay();

	static string GetCurrentUser();
	static bool IsRocksvvCluster();
	static bool IsIceCluster();
	static bool IsRocksvvClusterRootNode();
	static bool IsIceClusterRootNode();
	static bool IsLinux();
	static string GetHomePath();
	static bool IsUserPath(string path);
	static string ExpandUserPath(string path);
	static void BringWallmonServerWindowToFront();
	static string RunCommand(string cmd);
	static string RunCommand(const char *cmd);
};

#endif /* SYSTEM_H_ */
