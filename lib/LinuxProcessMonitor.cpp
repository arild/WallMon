#include "LinuxProcessMonitor.h"

LinuxProcessMonitor::LinuxProcessMonitor()
{
	_jiffy = sysconf(_SC_CLK_TCK);
	_buffer = new char[300]; // size is 200 but we use 300 for future versions (ubuntu version for example has many more fields...)
	memset(_buffer, 0, 300);

	_procstat = NULL;
	_procstatm = NULL;
	_procio = NULL;
	_utime = _stime = _rchar = _wchar = 0;
	_updateTime = 0.;
}

LinuxProcessMonitor::~LinuxProcessMonitor()
{
	if (_procstat)
		fclose(_procstat);
	if (_procstatm)
		fclose(_procstatm);
	if (_procio)
		fclose(_procio);
}

bool LinuxProcessMonitor::open(int pid)
{
	stringstream sstat, sstatm;
	sstat << "/proc/" << pid << "/stat";
	sstatm << "/proc/" << pid << "/statm";
	_procstat = fopen(sstat.str().c_str(), "r");
	_procstatm = fopen(sstatm.str().c_str(), "r");

	if (!_procstat || !_procstatm) {
		//cout << "Error opening files..." << endl;
		return false;
	}
	return true;
}

bool LinuxProcessMonitor::OpenIo(int pid)
{
	stringstream sio;
	sio << "/proc/" << pid << "/io";
	_procio = fopen(sio.str().c_str(), "r");
	if (!_procio) {
		//cout << "Error opening io files..." << endl;
		return false;
	}
	return true;
}

double GetTimeInSec()
{
	struct timeval t;
	gettimeofday(&t, NULL);

	return (double) t.tv_sec + 0.000001 * (double) t.tv_usec;
}

void LinuxProcessMonitor::update()
{
	_prevUserTime = _utime;
	_prevSystemTime = _stime;
	_prevUpdateTime = _updateTime;
	_prevTotalNetworkRead = _rchar;
	_prevTotalNetworkWrite = _wchar;
	_updateTime = GetTimeInSec();

	if (_procstat != NULL) {
		rewind(_procstat);
		fread(_buffer, 1, 300, _procstat);
		sscanf(
				_buffer,
				"%d %s %c %d %d %d %d %d %lu %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %lu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %d %d",
				&_pid, _comm, &_state, &_ppid, &_pgrp, &_session, &_tty_nr, &_tpgid, &_flags, &_minflt,
				&_cminflt, &_majflt, &_cmajflt, &_utime, &_stime, &_cutime, &_cstime, &_priority,
				&_nice, &_num_threads, &_itrealvalue, &_starttime, &_vsize, &_rss, &_rsslim,
				&_startcode, &_endcode, &_startstack, &_kstkesp, &_kstkeip, &_signal, &_blocked,
				&_sigignore, &_sigcatch, &_wchan, &_nswap, &_cnswap, &_exit_signal, &_processor);
	}
	if (_procstatm != NULL) {
		rewind(_procstatm);
		fread(_buffer, 1, 300, _procstatm);
		sscanf(_buffer, "%lu %lu %lu %lu %lu %lu %lu", &_size, &_resident, &_share, &_text, &_lib,
				&_data, &_dt);
	}
	if (_procio != NULL) {
		rewind(_procio);
		fread(_buffer, 1, 300, _procio);
		sscanf(_buffer, "%*s %lu %*s %lu %*s %lu %*s %lu %*s %lu %*s %lu %*s %lu", &_rchar, &_wchar,
				&_syscr, &_syscw, &_read_bytes, &_write_bytes, &_cancelled_write_bytes);
	}
}

void LinuxProcessMonitor::printAll()
{
	double time = GetTimeInSec();
	cout << fixed << setprecision(8) << time << ": ";

	printf(
			"%d %s %c %d %d %d %d %d %lu %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %lu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %d %d ",
			_pid, _comm, _state, _ppid, _pgrp, _session, _tty_nr, _tpgid, _flags, _minflt,
			_cminflt, _majflt, _cmajflt, _utime, _stime, _cutime, _cstime, _priority, _nice,
			_num_threads, _itrealvalue, _starttime, _vsize, _rss, _rsslim, _startcode, _endcode,
			_startstack, _kstkesp, _kstkeip, _signal, _blocked, _sigignore, _sigcatch, _wchan,
			_nswap, _cnswap, _exit_signal, _processor);

	printf("%lu %lu %lu %lu %lu %lu %lu ", _size, _resident, _share, _text, _lib, _data, _dt);

	cout << fixed << setprecision(5) << getUserTime() << " " << getSystemTime() << " "
			<< getDataSize() << endl;
}

double LinuxProcessMonitor::getUserTime()
{
	double seconds = (double) _utime / (double) _jiffy;
	return seconds;
}

double LinuxProcessMonitor::getSystemTime()
{
	double seconds = (double) _stime / (double) _jiffy;
	return seconds;
}

double LinuxProcessMonitor::getUserCPULoad()
{

	double load = (((double) _utime - (double) _prevUserTime) / (double) _jiffy) / (_updateTime
			- _prevUpdateTime);

	return load * 100.0;
}

double LinuxProcessMonitor::getSystemCPULoad()
{

	double load = (((double) _stime - (double) _prevSystemTime) / (double) _jiffy) / (_updateTime
			- _prevUpdateTime);

	return load * 100.0;
}

double LinuxProcessMonitor::getTotalProgramSize()
{

	return (double) (_size * 4096.0) / (1024. * 1024.0);
}

double LinuxProcessMonitor::getCodeSize()
{

	return (double) (_text * 4096.0) / (1024. * 1024.0);
}

double LinuxProcessMonitor::getSharedPages()
{

	return (double) (_share * 4096.0) / (1024. * 1024.0);
}

double LinuxProcessMonitor::getDataSize()
{

	return (double) (_data * 4096.0) / (1024. * 1024.0);
}

double LinuxProcessMonitor::getResidentSetSize()
{

	return (double) (_resident * 4096.0) / (1024. * 1024.0);
}

int LinuxProcessMonitor::pid()
{
	return _pid;
}
char* LinuxProcessMonitor::comm()
{
	return _comm;
}
char LinuxProcessMonitor::state()
{
	return _state;
}
int LinuxProcessMonitor::ppid()
{
	return _ppid;
}
int LinuxProcessMonitor::pgrp()
{
	return _pgrp;
}
int LinuxProcessMonitor::session()
{
	return _session;
}
int LinuxProcessMonitor::tty_nr()
{
	return _tty_nr;
}
int LinuxProcessMonitor::tpgid()
{
	return _tpgid;
}
unsigned long LinuxProcessMonitor::flags()
{
	return _flags;
}
unsigned long LinuxProcessMonitor::minflt()
{
	return _minflt;
}
unsigned long LinuxProcessMonitor::cminflt()
{
	return _cminflt;
}
unsigned long LinuxProcessMonitor::majflt()
{
	return _majflt;
}
unsigned long LinuxProcessMonitor::cmajflt()
{
	return _cmajflt;
}
unsigned long LinuxProcessMonitor::utime()
{
	return _utime;
}
unsigned long LinuxProcessMonitor::stime()
{
	return _stime;
}
long int LinuxProcessMonitor::cutime()
{
	return _cutime;
}
long int LinuxProcessMonitor::cstime()
{
	return _cstime;
}
long int LinuxProcessMonitor::priority()
{
	return _priority;
}
long int LinuxProcessMonitor::nice()
{
	return _nice;
}
long int LinuxProcessMonitor::num_threads()
{
	return _num_threads;
}
long int LinuxProcessMonitor::itrealvalue()
{
	return _itrealvalue;
}
unsigned long LinuxProcessMonitor::starttime()
{
	return _starttime;
}
unsigned long LinuxProcessMonitor::vsize()
{
	return _vsize;
}
long int LinuxProcessMonitor::rss()
{
	return _rss;
}
unsigned long LinuxProcessMonitor::rsslim()
{
	return _rsslim;
}
unsigned long LinuxProcessMonitor::startcode()
{
	return _startcode;
}
unsigned long LinuxProcessMonitor::endcode()
{
	return _endcode;
}
unsigned long LinuxProcessMonitor::startstack()
{
	return _startstack;
}
unsigned long LinuxProcessMonitor::kstkesp()
{
	return _kstkesp;
}
unsigned long LinuxProcessMonitor::kstkeip()
{
	return _kstkeip;
}
unsigned long LinuxProcessMonitor::signal()
{
	return _signal;
}
unsigned long LinuxProcessMonitor::blocked()
{
	return _blocked;
}
unsigned long LinuxProcessMonitor::sigignore()
{
	return _sigignore;
}
unsigned long LinuxProcessMonitor::sigcatch()
{
	return _sigcatch;
}
unsigned long LinuxProcessMonitor::wchan()
{
	return _wchan;
}
unsigned long LinuxProcessMonitor::nswap()
{
	return _nswap;
}
unsigned long LinuxProcessMonitor::cnswap()
{
	return _cnswap;
}
int LinuxProcessMonitor::exit_signal()
{
	return _exit_signal;
}
int LinuxProcessMonitor::processor()
{
	return _processor;
}

long LinuxProcessMonitor::jiffy()
{
	return _jiffy;
}

unsigned long LinuxProcessMonitor::size()
{
	return _size;
}
unsigned long LinuxProcessMonitor::resident()
{
	return _resident;
}
unsigned long LinuxProcessMonitor::share()
{
	return _share;
}
unsigned long LinuxProcessMonitor::text()
{
	return _text;
}
unsigned long LinuxProcessMonitor::lib()
{
	return _lib;
}
unsigned long LinuxProcessMonitor::data()
{
	return _data;
}
unsigned long LinuxProcessMonitor::dt()
{
	return _dt;
}

unsigned long LinuxProcessMonitor::rchar()
{
	return _rchar;
}

unsigned long LinuxProcessMonitor::wchar()
{
	return _wchar;
}

unsigned long LinuxProcessMonitor::syscr()
{
	return _syscr;
}

unsigned long LinuxProcessMonitor::syscw()
{
	return _syscw;
}

unsigned long LinuxProcessMonitor::read_bytes()
{
	return _read_bytes;
}

unsigned long LinuxProcessMonitor::write_bytes()
{
	return _write_bytes;
}

unsigned long LinuxProcessMonitor::cancelled_write_bytes()
{
	return _cancelled_write_bytes;
}

unsigned long LinuxProcessMonitor::GetNetworkInInBytes()
{
	return _rchar - _prevTotalNetworkRead;
}

unsigned long LinuxProcessMonitor::GetNetworkOutInBytes()
{
	return _wchar - _prevTotalNetworkWrite;
}
















