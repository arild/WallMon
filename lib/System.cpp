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
#include <iostream>
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
	char buf[100];
	string cmd = "pgrep " + processName;
	FILE *fp = popen(cmd.c_str(), "r");
	if (fp == NULL) {
		return -1;
	}
	void *res = fgets(buf, 100, fp);
	pclose(fp);
	if (res == NULL)
		// process name not found
		return -1;
	return atoi(buf);
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

int _RunCommand(string cmd, char *buf, int len)
{
	FILE *fp = popen(cmd.c_str(), "r");
	if (fp == NULL)
		return -1;
	void *res = fgets(buf, len, fp);
	pclose(fp);
	if (res == NULL)
		return -1;
	return 0;
}

vector<int> *System::System::GetAllPids()
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

int System::GetNumCores()
{
	char buf[4096];
	string cmd = "cat /proc/cpuinfo | grep processor | wc -l";
	_RunCommand(cmd, buf, 4096);
	return atoi(buf);
}

int System::GetNumCoresExcludeHyperThreading()
{
	char buf[128];
	string cmd = "cat /proc/cpuinfo | grep -m 1 'cpu cores' | awk '{print $4}'";
	_RunCommand(cmd, buf, 128);
	return atoi(buf);
}

int System::GetTotalMemory()
{
	char buf[4096];
	string cmd = "cat /proc/meminfo | grep MemTotal: | awk '{print $2}'";
	_RunCommand(cmd, buf, 4096);
	return atoi(buf);
}

bool System::HasSupportForProcPidIo()
{
	if (System::IsRocksvvCluster())
		return false;
	return true;
}

bool System::IsRocksvvCluster()
{
	char buf[4096];
	string cmd = "hostname";
	_RunCommand(cmd, buf, 4096);
	string rocks = "rocksvv.cs.uit.no";
	if (strncmp(buf, rocks.c_str(), rocks.length()) == 0)
		return true;
	if (strncmp(buf, "tile-", 5) == 0)
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

void System::ExportDisplayToLocalhost()
{
	string cmd = "export DISPLAY=localhost:0";
	char buf[1024];
	_RunCommand(cmd, buf, 1024);
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

string System::RunCommand(string cmd)
{
	char buf[4096];

	FILE *fp = popen(cmd.c_str(), "r");
	if (fp == NULL)
		return NULL;
	void *res = fgets(buf, 4096, fp);
	pclose(fp);
	if (res == NULL)
		return NULL;

	// Remove '\n' before returning result
	string retval = (string)buf;
	return retval.erase(retval.length() - 1, 1);
}

string System::RunCommand(char *cmd)
{
	return RunCommand((string)cmd);
}



















