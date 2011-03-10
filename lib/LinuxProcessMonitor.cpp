#include "LinuxProcessMonitor.h"

LinuxProcessMonitor::LinuxProcessMonitor()
{
	_jiffy = sysconf(_SC_CLK_TCK);
	_buffer = new char[300]; // size is 200 but we use 300 for future versions (ubuntu version for example has many more fields...)
	memset(_buffer, 0, 300);

	_procstat = NULL;
	_procstatm = NULL;
}

LinuxProcessMonitor::~LinuxProcessMonitor()
{
	if (_procstat)
		fclose(_procstat);
	if (_procstatm)
		fclose(_procstatm);
}

bool LinuxProcessMonitor::open(int pid)
{
	stringstream ssprocstat, ssprocstatm;

	ssprocstat << "/proc/" << pid << "/stat";
	ssprocstatm << "/proc/" << pid << "/statm";

	_procstatfile = ssprocstat. str();
	_procstatmfile = ssprocstatm.str();

	_procstat = fopen(_procstatfile.c_str(), "r");
	_procstatm = fopen(_procstatmfile.c_str(), "r");

	if (!_procstat || !_procstatm) {

		cout << "Error opening files..." << endl;
		return false;
	}
	return true;
}

double getTime()
{ // returns the current time in seconds

	struct timeval t;
	gettimeofday(&t, NULL);

	return (double) t.tv_sec + 0.000001 * (double) t.tv_usec;
}

void LinuxProcessMonitor::update()
{

	_prevutime = _utime;
	_prevstime = _stime;
	_prevupdatetime = _updatetime;
	_updatetime = getTime();

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

	rewind(_procstatm);
	fread(_buffer, 1, 300, _procstatm);
	sscanf(_buffer, "%lu %lu %lu %lu %lu %lu %lu", &_size, &_resident, &_share, &_text, &_lib,
			&_data, &_dt);
}

void LinuxProcessMonitor::printAll()
{

	double time = getTime();
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

	double load = (((double) _utime - (double) _prevutime) / (double) _jiffy) / (_updatetime
			- _prevupdatetime);

	return load * 100.0;
}

double LinuxProcessMonitor::getSystemCPULoad()
{

	double load = (((double) _stime - (double) _prevstime) / (double) _jiffy) / (_updatetime
			- _prevupdatetime);

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

