/*
 * LinuxProcessMonitorLight.h
 *
 *  Created on: Jul 2, 2011
 *      Author: arild
 */

#ifndef LINUXPROCESSMONITORLIGHT_H_
#define LINUXPROCESSMONITORLIGHT_H_

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <string>
//#include <linux/limits.h>
#include <sys/times.h>
#include <iomanip>

using namespace std;

class LinuxProcessMonitorLight {

public:

	LinuxProcessMonitorLight();
	virtual ~LinuxProcessMonitorLight();

	bool Open(int pid=-1);
	bool Update();
	double SecondsSinceLastUpdate();

	// stat
	double GetUserTime();
	double GetSystemTime();
	double GetUserCpuLoad(); // in percentage
	double GetSystemCpuLoad(); // in percentage
	string GetProcessName();
	unsigned long utime();
	unsigned long stime();
	int pid();
	char* comm();
	long jiffy();
	unsigned int numthreads();

	// statm
	double GetTotalProgramSize(); // in megabytes
	unsigned long size();

	// io
	unsigned long GetNetworkInInBytes();
	unsigned long GetNetworkOutInBytes();
	unsigned long rchar();
	unsigned long wchar();

private:
	// /proc/<pid>/stat variables
	int _pid;
	char _comm[4096];
	unsigned long _utime;
	unsigned long _stime;
	unsigned int _numThreads;

	// /proc/<pid>/statm variables
	unsigned long _size;

	// /proc/<pid>/io variables
	unsigned long _rchar;
	unsigned long _wchar;

	long _jiffy; // (USER_HZ)
	long _timesinceboot;
	char *_buffer;

	unsigned long _prevUserTime;
	unsigned long _prevSystemTime;
	unsigned long _prevTotalNetworkRead;
	unsigned long _prevTotalNetworkWrite;
	double _updateTime;
	double _prevUpdateTime;

	string _procstatfile;
	string _procstatmfile;
	string _prociofile;

	FILE *_procstat;
	FILE *_procstatm;
	FILE *_procio;

	unsigned long _totalNumBytesRead;
};
#endif /* LINUXPROCESSMONITORLIGHT_H_ */
