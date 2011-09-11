/*
 * System.cpp
 *
 *  Created on: Feb 13, 2011
 *      Author: arild
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h> // gettimeofday()
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream> // ifstream()
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include "System.h"
#include <boost/algorithm/string.hpp>
#include <glog/logging.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

string *hostname = NULL;

// From: http://www.qgd.unizh.ch/~dpotter/howto/daemonize
void System::Daemonize()
{
	pid_t pid, sid;

	/* already a daemon */
	if (getppid() == 1)
		return;

	/* Fork off the parent process */
	pid = fork();
	if (pid < 0)
		LOG(FATAL) << "fork failed";

	/* If we got a good PID, then we can exit the parent process. */
	if (pid > 0)
		exit(EXIT_SUCCESS);

	/* At this point we are executing as the child process */

	/* Change the file mode mask */
	umask(0);

	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0)
		LOG(FATAL) << "setting sid failed";

	/* Change the current working directory.  This prevents the current
	 directory from being locked; hence not being able to remove it. */
	if ((chdir("/")) < 0)
		LOG(FATAL) << "failed changing directory";

	/* Redirect standard files to /dev/null */
	if (!freopen("/dev/null", "r", stdin) || !freopen("/dev/null", "w", stdout) || freopen(
			"/dev/null", "w", stderr))
		LOG(WARNING) << "failed redirecting std out and/or std in";
}

int System::GetPid(string processName)
{
	string cmd = "pgrep " + processName;
	processName = RunCommand(cmd);
	if (processName.empty())
		// process name not found
		return -1;
	return atoi(processName.c_str());
}

void _TokenizeFile(FILE *file, vector<int> *v)
{
	char *word;
	char buf[101];
	buf[100] = 0;

	while (!feof(file)) {
		/* Skip non-letters */
		fscanf(file, "%*[^a-zA-Z0-9'_]");
		/* Scan up to 100 letters */
		if (fscanf(file, "%100[a-zA-Z0-9'_]", buf) == 1) {
			v->push_back(atoi(buf));
		}
	}
}

vector<int> *System::GetAllPids()
{
	string cmd = "ps ax | awk '{print $1}'";
	FILE *fp = popen(cmd.c_str(), "r");
	if (fp == NULL)
		return NULL;
	vector<int> *v = new vector<int> ;
	_TokenizeFile(fp, v);
	pclose(fp);
	return v;
}

double System::GetTimeInSec()
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return (double) t.tv_sec + 0.000001 * (double) t.tv_usec;
}

double System::GetTimeInMsec()
{
//	struct timeval t;
//	gettimeofday(&t, NULL);
//	return ((double)t.tv_sec * 1000.0) + (double)(0.001 * (double)t.tv_usec);
	return GetTimeInSec() * 1000.0;
}

string &System::GetHostname()
{
	if (hostname == NULL) {
		char buf[200];
		gethostname(buf, 200);
		hostname = new string(buf);
	}
	return *hostname;
}

/**
 * Takes Hyper-threading and duplicated multicore technology into account.
 *
 * For example: A 2-way quad core with Hyper-threading will have 16 logical cores
 */
int System::GetNumLogicalCores()
{
	string cmd = "cat /proc/cpuinfo | grep processor | wc -l";
	return atoi(RunCommand(cmd).c_str());
}

/**
 * Excludes Hyper-threading and duplicated multicore technology into account.
 *
 * For example: A 2-way quad core with Hyper-threading will have 4 real cores
 */

int System::GetNumRealCores()
{
	string cmd = "cat /proc/cpuinfo | grep -m 1 'cpu cores' | awk '{print $4}'";
	return atoi(RunCommand(cmd).c_str());
}

int System::GetTotalMemory()
{
	string cmd = "cat /proc/meminfo | grep MemTotal: | awk '{print $2}'";
	return atoi(RunCommand(cmd).c_str());
}

bool System::IsRocksvvCluster()
{
	string hostname = RunCommand("hostname");;
	string rocks = "rocksvv.cs.uit.no";
	if (rocks.compare(hostname) == 0)
		return true;
	if (hostname.compare("tile-") == 0)
		return true;
	return false;
}

bool System::IsValidIpAddress(string ipAddress)
{
	struct sockaddr_in sa;
	int result = inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr));
	return result != 0;
}

vector<string> System::HostnameToIpAddress(string hostname)
{
	struct hostent *he;
	struct in_addr a;
	vector<string> result;

	he = gethostbyname(hostname.c_str());
	if (he) {
		while (*he->h_addr_list) {
			bcopy(*he->h_addr_list++, (char *) &a, sizeof(a));
			result.push_back((string)inet_ntoa(a));
		}
	}
	return result;
}

void System::AttachToLocalDisplay()
{
	RunCommand("export DISPLAY=localhost:0");
}

string System::GetCurrentUser()
{
	return RunCommand("whoami");
}

bool System::IsRocksvvClusterRootNode()
{
	if (GetHostname().compare("rocksvv.cs.uit.no") == 0)
		return true;
	return false;
}

bool System::IsIceClusterRootNode()
{
	if (GetHostname().compare("ice.cs.uit.no") == 0)
		return true;
	return false;
}

bool System::IsLinux()
{
	if (RunCommand("uname").compare("Linux") == 0)
		return true;
	return false;
}

string System::GetHomePath()
{
	string homeDir = "Users";
	if (IsLinux())
		homeDir = "home";
	return "/" + homeDir + "/" + GetCurrentUser();
}

bool System::IsUserPath(string path)
{
	if (path.compare(0, 1, "~") == 0)
		return true;
	return false;
}

string System::ExpandUserPath(string path)
{
	return path.replace(0, 1, GetHomePath());
}

void System::BringWallmonServerWindowToFront()
{
	RunCommand("wmctrl -r 'WallMon' -b toggle,above");
}

string System::RunCommand(string cmd)
{
	char buf[4096];

	FILE *fp = popen(cmd.c_str(), "r");
	if (fp == NULL)
		return "";
	void *res = fgets(buf, 4096, fp);
	pclose(fp);
	if (res == NULL)
		return "";

	// Remove '\n' before returning result
	string retval = (string)buf;
	return retval.erase(retval.length() - 1, 1);
}

string System::RunCommand(char *cmd)
{
	return RunCommand((string)cmd);
}



















