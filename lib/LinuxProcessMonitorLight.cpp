/*
 * LinuxProcessMonitorLightLight.cpp
 *
 *  Created on: Jul 2, 2011
 *      Author: arild
 */

#include "LinuxProcessMonitorLight.h"
#include "System.h"
#include <glog/logging.h>

LinuxProcessMonitorLight::LinuxProcessMonitorLight()
{
	_jiffy = sysconf(_SC_CLK_TCK);
	// size is 200 but we use 300 for future versions (ubuntu version for example has many more fields...)
	memset(_bufStat, 0, 300);
	memset(_bufStatm, 0, 300);
	memset(_bufIo, 0, 300);

	_procstat = NULL;
	_procstatm = NULL;
	_procio = NULL;
	_utime = _stime = _rchar = _wchar = _syscr = _syscw = _read_bytes = _write_bytes = 0;
	_updateTime = 0.;
	_totalNumBytesRead = 0;
	_totalNumSamples = 0;

	_statReadProcfs = NULL;
	_statParseProcfs = NULL;
}

LinuxProcessMonitorLight::~LinuxProcessMonitorLight()
{
	if (_procstat)
		fclose(_procstat);
	if (_procstatm)
		fclose(_procstatm);
	if (_procio)
		fclose(_procio);
}

void LinuxProcessMonitorLight::SetStatistics(Stat<double> *readProcfs, Stat<double> *parseProcfs)
{
	_statReadProcfs = readProcfs;
	_statParseProcfs = parseProcfs;
}

bool LinuxProcessMonitorLight::Open(int pid)
{
	if (pid == -1)
		pid = getpid();

	stringstream sstat, sstatm, sio;

	sstat << "/proc/" << pid << "/stat";
	_procstat = fopen(sstat.str().c_str(), "r");

	sstatm << "/proc/" << pid << "/statm";
	_procstatm = fopen(sstatm.str().c_str(), "r");

	sio << "/proc/" << pid << "/io";
	_procio = fopen(sio.str().c_str(), "r");

	if (_procstat == NULL && _procstatm == NULL && _procio == NULL) {
		return false;
	}

	stringstream userss, datess;
	userss << "stat -c %U /proc/" << pid;
	_user = System::RunCommand(userss.str());
	datess << "date +\"%b%d %k:%M\" -r /proc/" << pid;
	_startTime = System::RunCommand(datess.str());

	return true;
}

bool LinuxProcessMonitorLight::Update()
{
	_prevMinflt = _minflt;
	_prevMajflt = _majflt;
	_prevUserTime = _utime;
	_prevSystemTime = _stime;
	_prevUpdateTime = _updateTime;
	_prevRchar = _rchar;
	_prevWchar = _wchar;
	_prevSyscr = _syscr;
	_prevSyscw = _syscw;
	_prevReadBytes = _read_bytes;
	_prevWriteBytes = _write_bytes;
	_updateTime = System::GetTimeInSec();
	_totalNumSamples += 1;

	if (_procstat != NULL) {
		rewind(_procstat);
		_totalNumBytesRead += fread(_bufStat, 1, 300, _procstat);
		if (ferror(_procstat))
			return false;

		sscanf(_bufStat,
				"%d %s %*c %*d %*d %*d %*d %*d %*u %lu %*lu %lu %*lu %lu %lu %*ld %*ld %*ld %*ld %d",
				&_pid, _comm, &_minflt, &_majflt, &_utime, &_stime, &_numThreads);
	}
	if (_procstatm != NULL) {
		rewind(_procstatm);
		_totalNumBytesRead += fread(_bufStatm, 1, 300, _procstatm);
		if (ferror(_procstatm))
			return false;

		sscanf(_bufStatm, "%lu", &_size);
	}
	if (_procio != NULL) {
		rewind(_procio);
		_totalNumBytesRead += fread(_bufIo, 1, 300, _procio);
		if (ferror(_procio))
			return false;

		sscanf(_bufIo, "%*s %lu %*s %lu %*s %lu %*s %lu %*s %lu %*s %lu", &_rchar, &_wchar, &_syscr, &_syscw, &_read_bytes, &_write_bytes);
	}

	double afterReadProcfs;
	if (_statReadProcfs != NULL) {
		afterReadProcfs = System::GetTimeInMsec();
		_statReadProcfs->Add(afterReadProcfs - _updateTime * (double)1000);
	}


	if (_statReadProcfs != NULL)
		_statParseProcfs->Add(System::GetTimeInMsec() - afterReadProcfs);

	return true;
}

double LinuxProcessMonitorLight::SecondsSinceLastUpdate()
{
	return System::GetTimeInSec() - _updateTime;
}

/**
 * Requires three or more calls to Update() for not being first time,
 * with only two calls, only the first interval has been measured
 */
bool LinuxProcessMonitorLight::IsFirstTime()
{
	if (_totalNumSamples < 3)
		return true;
	return false;
}

string LinuxProcessMonitorLight::GetUser()
{
	return _user;
}

string LinuxProcessMonitorLight::GetStartTime()
{
	return _startTime;
}

double LinuxProcessMonitorLight::GetTotalNumPageFaultsPerSec()
{
	unsigned long numFaultsSinceLastUpdate = (_minflt + _majflt) - (_prevMinflt + _prevMajflt);
	return numFaultsSinceLastUpdate / (double)(_updateTime - _prevUpdateTime);
}
double LinuxProcessMonitorLight::GetUserTime()
{
	double seconds = (double) _utime / (double) _jiffy;
	return seconds;
}

double LinuxProcessMonitorLight::GetSystemTime()
{
	double seconds = (double) _stime / (double) _jiffy;
	return seconds;
}

double LinuxProcessMonitorLight::GetUserCpuLoad()
{

	double load = (((double) _utime - (double) _prevUserTime) / (double) _jiffy) / (_updateTime
			- _prevUpdateTime);
	return load * 100.0;
}

double LinuxProcessMonitorLight::GetSystemCpuLoad()
{

	double load = (((double) _stime - (double) _prevSystemTime) / (double) _jiffy) / (_updateTime
			- _prevUpdateTime);
	return load * 100.0;
}

string LinuxProcessMonitorLight::GetProcessName()
{
	string processName = (string) comm();
	// Remove '(' and ')' from the process name returned from comm()
	processName.erase(processName.length() - 1, 1);
	processName.erase(0, 1);
	return processName;
}

double LinuxProcessMonitorLight::GetTotalProgramSize()
{
	return  GetTotalProgramSizeBytes() / (double)(1024 * 1024);
}

uint64_t LinuxProcessMonitorLight::GetTotalProgramSizeBytes()
{
	return _size * 4096;
}

unsigned long LinuxProcessMonitorLight::minflt()
{
	return _minflt;
}
unsigned long LinuxProcessMonitorLight::majflt()
{
	return _majflt;
}

int LinuxProcessMonitorLight::pid()
{
	return _pid;
}

char* LinuxProcessMonitorLight::comm()
{
	return _comm;
}

unsigned long LinuxProcessMonitorLight::utime()
{
	return _utime;
}
unsigned long LinuxProcessMonitorLight::stime()
{
	return _stime;
}

long LinuxProcessMonitorLight::jiffy()
{
	return _jiffy;
}

unsigned int LinuxProcessMonitorLight::numthreads()
{
	return _numThreads;
}

unsigned long LinuxProcessMonitorLight::size()
{
	return _size;
}

unsigned long LinuxProcessMonitorLight::GetTotalIoInBytes()
{
	return _rchar - _prevRchar;
}

unsigned long LinuxProcessMonitorLight::GetTotalIoOutBytes()
{
	return _wchar - _prevWchar;
}

unsigned long LinuxProcessMonitorLight::GetNetworkInBytes()
{
	unsigned long ioInBytes = GetTotalIoInBytes();
	unsigned long storageInBytes = GetStorageInBytes();
	if (storageInBytes > ioInBytes)
		return 0;
	return ioInBytes - storageInBytes;
}

unsigned long LinuxProcessMonitorLight::GetNetworkOutBytes()
{
	unsigned long ioOutBytes = GetTotalIoOutBytes();
	unsigned long storageOutBytes = GetStorageOutBytes();
	if (storageOutBytes > ioOutBytes)
		return 0;
	return ioOutBytes - storageOutBytes;
}

unsigned long LinuxProcessMonitorLight::GetStorageInBytes()
{
	return _read_bytes - _prevReadBytes;
}

unsigned long LinuxProcessMonitorLight::GetStorageOutBytes()
{
	return _write_bytes - _prevWriteBytes;
}

float LinuxProcessMonitorLight::GetNumReadSysCallsPerSec()
{
	return (_syscr - _prevSyscr) / (float)(_updateTime - _prevUpdateTime);
}

float LinuxProcessMonitorLight::GetNumWriteSysCallsPerSec()
{
	return (_syscw - _prevSyscw) / (float)(_updateTime - _prevUpdateTime);
}



unsigned long LinuxProcessMonitorLight::rchar()
{
	return _rchar;
}

unsigned long LinuxProcessMonitorLight::wchar()
{
	return _wchar;
}

unsigned long LinuxProcessMonitorLight::syscr()
{
	return _syscr;
}

unsigned long LinuxProcessMonitorLight::syscw()
{
	return _syscw;
}

unsigned long LinuxProcessMonitorLight::readbytes()
{
	return _read_bytes;
}

unsigned long LinuxProcessMonitorLight::writechar()
{
	return _write_bytes;
}



