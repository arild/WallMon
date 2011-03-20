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
#include <glog/logging.h>
#include "System.h"
#include <boost/algorithm/string.hpp>

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
	if (pid < 0) {
		exit(EXIT_FAILURE);
	}
	/* If we got a good PID, then we can exit the parent process. */
	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	/* At this point we are executing as the child process */

	/* Change the file mode mask */
	umask(0);

	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0) {
		exit(EXIT_FAILURE);
	}

	/* Change the current working directory.  This prevents the current
	 directory from being locked; hence not being able to remove it. */
	if ((chdir("/")) < 0) {
		exit(EXIT_FAILURE);
	}

	/* Redirect standard files to /dev/null */
	if (!freopen("/dev/null", "r", stdin) || !freopen("/dev/null", "w", stdout) || freopen(
			"/dev/null", "w", stderr))
		LOG(ERROR) << "freopen failed";
}

int System::GetPid(string processName)
{
	char buf[100];
	string cmd = "pgrep " + processName;
	FILE *fp = popen(cmd.c_str(), "r");
	if (fp == NULL) {
		LOG(ERROR) << "failed to run command";
		return -1;
	}
	void *res = fgets(buf, 100, fp);
	pclose(fp);
	if (res == NULL)
		// process name not found
		return -1;
	return atoi(buf);
}

void tokenize_file(FILE *file, list<int> *list)
{
	char *word;
	char buf[101];
	buf[100] = 0;

	while (!feof(file)) {
		/* Skip non-letters */
		fscanf(file, "%*[^a-zA-Z0-9'_]");
		/* Scan up to 100 letters */
		if (fscanf(file, "%100[a-zA-Z0-9'_]", buf) == 1) {
			list->push_back(atoi(buf));
		}
	}
}

list<int> *System::System::GetAllPids()
{
	string cmd = "ps ax | awk '{print $1}'";
	FILE *fp = popen(cmd.c_str(), "r");
	if (fp == NULL) {
		LOG(ERROR) << "failed to run command";
	}
	list<int> *l = new list<int> ;
	tokenize_file(fp, l);
	pclose(fp);
	return l;
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

