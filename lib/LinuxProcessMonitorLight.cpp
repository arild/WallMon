/*
 * LinuxProcessMonitorLightLight.cpp
 *
 *  Created on: Jul 2, 2011
 *      Author: arild
 */

#include "LinuxProcessMonitorLight.h"
#include "System.h"

LinuxProcessMonitorLight::LinuxProcessMonitorLight()
{
	_jiffy = sysconf(_SC_CLK_TCK);
	_buffer = new char[300]; // size is 200 but we use 300 for future versions (ubuntu version for example has many more fields...)
	memset(_buffer, 0, 300);

	_procstat = NULL;
	_procstatm = NULL;
	_procio = NULL;
	_utime = _stime = _rchar = _wchar = 0;
	_updateTime = 0.;
	_totalNumBytesRead = 0;
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

bool LinuxProcessMonitorLight::Open(int pid)
{
	if (pid == -1)
		pid = getpid();

	stringstream sstat, sstatm, sio;

	sstat << "/proc/" << pid << "/stat";
	_procstat = fopen(sstat.str().c_str(), "r");
	if (_procstat == NULL)
		return false;

	sstatm << "/proc/" << pid << "/statm";
	_procstatm = fopen(sstatm.str().c_str(), "r");
	if (_procstatm == NULL)
		return false;

	sio << "/proc/" << pid << "/io";
	_procio = fopen(sio.str().c_str(), "r");
	if (_procio == NULL)
		return false;

	return true;
}

void LinuxProcessMonitorLight::Update()
{
	_prevUserTime = _utime;
	_prevSystemTime = _stime;
	_prevUpdateTime = _updateTime;
	_prevTotalNetworkRead = _rchar;
	_prevTotalNetworkWrite = _wchar;
	_updateTime = System::GetTimeInSec();

	rewind(_procstat);
	_totalNumBytesRead += fread(_buffer, 1, 300, _procstat);
	sscanf(_buffer, "%d %s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu", &_pid, _comm,
			&_utime, &_stime);

	rewind(_procstatm);
	_totalNumBytesRead += fread(_buffer, 1, 300, _procstatm);
	sscanf(_buffer, "%lu", &_size);

	rewind(_procio);
	_totalNumBytesRead += fread(_buffer, 1, 300, _procio);
	sscanf(_buffer, "%*s %lu %*s %lu", &_rchar, &_wchar);
}

double LinuxProcessMonitorLight::SecondsSinceLastUpdate()
{
	return System::GetTimeInSec() - _updateTime;
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
	string processName = (string)comm();
	// Remove '(' and ')' from the process name returned from comm()
	processName.erase(processName.length() - 1, 1);
	processName.erase(0, 1);
	return processName;
}

double LinuxProcessMonitorLight::GetTotalProgramSize()
{
	return (double) (_size * 4096.0) / (1024. * 1024.0);
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

unsigned long LinuxProcessMonitorLight::size()
{
	return _size;
}

unsigned long LinuxProcessMonitorLight::rchar()
{
	return _rchar;
}

unsigned long LinuxProcessMonitorLight::wchar()
{
	return _wchar;
}

unsigned long LinuxProcessMonitorLight::GetNetworkInInBytes()
{
	return _rchar - _prevTotalNetworkRead;
}

unsigned long LinuxProcessMonitorLight::GetNetworkOutInBytes()
{
	return _wchar - _prevTotalNetworkWrite;
}

