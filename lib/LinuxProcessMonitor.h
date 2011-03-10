#ifndef __LINUX_PROCESS_MONITOR__
#define __LINUX_PROCESS_MONITOR__

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <linux/limits.h>
#include <sys/times.h>
#include <iomanip>

using namespace std;

class LinuxProcessMonitor {

public:

	LinuxProcessMonitor();
	virtual ~LinuxProcessMonitor();

	bool open(int pid);
	void update();
	void printAll();

	int pid();
	char* comm();
	char state();
	int ppid();
	int pgrp();
	int session();
	int tty_nr();
	int tpgid();
	unsigned long flags();
	unsigned long minflt();
	unsigned long cminflt();
	unsigned long majflt();
	unsigned long cmajflt();
	unsigned long utime();
	unsigned long stime();
	long int cutime();
	long int cstime();
	long int priority();
	long int nice();
	long int num_threads();
	long int itrealvalue();
	unsigned long starttime();
	unsigned long vsize();
	long int rss();
	unsigned long rsslim();
	unsigned long startcode();
	unsigned long endcode();
	unsigned long startstack();
	unsigned long kstkesp();
	unsigned long kstkeip();
	unsigned long signal();
	unsigned long blocked();
	unsigned long sigignore();
	unsigned long sigcatch();
	unsigned long wchan();
	unsigned long nswap();
	unsigned long cnswap();
	int exit_signal();
	int processor();

	long jiffy();

	double getUserTime();
	double getSystemTime();

	double getUserCPULoad(); // in percentage
	double getSystemCPULoad(); // in percentage

	double getTotalProgramSize(); // in megabytes
	double getCodeSize(); // in megabytes
	double getDataSize(); // in megabytes (data + stack)
	double getResidentSetSize(); // in megabytes
	double getSharedPages(); // in megabytes

	unsigned long size();
	unsigned long resident();
	unsigned long share();
	unsigned long text();
	unsigned long lib();
	unsigned long data();
	unsigned long dt();

private:

	// stat variables
	int _pid;
	char _comm[PATH_MAX];
	char _state;
	int _ppid;
	int _pgrp;
	int _session;
	int _tty_nr;
	int _tpgid;
	unsigned long _flags;
	unsigned long _minflt;
	unsigned long _cminflt;
	unsigned long _majflt;
	unsigned long _cmajflt;
	unsigned long _utime;
	unsigned long _stime;
	long int _cutime;
	long int _cstime;
	long int _priority;
	long int _nice;
	long int _num_threads;
	long int _itrealvalue;
	unsigned long _starttime;
	unsigned long _vsize;
	long int _rss;
	unsigned long _rsslim;
	unsigned long _startcode;
	unsigned long _endcode;
	unsigned long _startstack;
	unsigned long _kstkesp;
	unsigned long _kstkeip;
	unsigned long _signal;
	unsigned long _blocked;
	unsigned long _sigignore;
	unsigned long _sigcatch;
	unsigned long _wchan;
	unsigned long _nswap;
	unsigned long _cnswap;
	int _exit_signal;
	int _processor;

	long _jiffy; // (USER_HZ)
	long _timesinceboot;

	string _procstatfile;
	string _procstatmfile;

	char *_buffer;

	unsigned long _prevutime;
	unsigned long _prevstime;
	double _updatetime;
	double _prevupdatetime;

	// statm variables
	unsigned long _size;
	unsigned long _resident;
	unsigned long _share;
	unsigned long _text;
	unsigned long _lib;
	unsigned long _data;
	unsigned long _dt;

	/*
	 unsigned long      _rt_priority;
	 unsigned int       _policy;
	 unsigned long long _delayacct_blkio_ticks;

	 unsigned long      _guest_time,
	 long int       	   _cguest_time
	 */

	FILE *_procstat;
	FILE *_procstatm;

};

#endif

