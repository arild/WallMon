
#include "TableItem.h"

typedef boost::mutex::scoped_lock scoped_lock;

TableItem::TableItem(string key_)
{
	key = key_;
	_score = 0;
	highlightNumber = -1;
}

void TableItem::AddBoid(BoidSharedContext *boid)
{
	scoped_lock(_mutex);
	if (_boids.size() == 0) {
		r = boid->red;
		g = boid->green;
		b = boid->blue;
	}
	_boids.push_back(boid);
}

vector<BoidSharedContext *> TableItem::GetBoids()
{
	scoped_lock(_mutex);
	return _boids;
}

float TableItem::GetScore()
{
	return _score;
}

float TableItem::SetScore(float score)
{
	_score = score;
}

void TableItem::SetProcName(string procName)
{
	scoped_lock(_mutex);
	_procName = procName;
}

string TableItem::GetProcName()
{
	scoped_lock(_mutex);
	return _procName;
}

void TableItem::SetHostName(string hostName)
{
	scoped_lock(_mutex);
	_hostName = hostName;
}

string TableItem::GetHostName()
{
	scoped_lock(_mutex);
	return _hostName;
}

void TableItem::SetPid(string pid)
{
	scoped_lock(_mutex);
	_pid = pid;
}

string TableItem::GetPid()
{
	scoped_lock(_mutex);
	return _pid;
}

void TableItem::SetUser(string user)
{
	scoped_lock(_mutex);
	_user = user;
}

string TableItem::GetUser()
{
	scoped_lock(_mutex);
	return _user;
}

void TableItem::SetStartTime(string time)
{
	scoped_lock(_mutex);
	_time = time;
}

string TableItem::GetStartTime()
{
	scoped_lock(_mutex);
	return _time;
}

void TableItem::SetNumThreads(string numThreads)
{
	scoped_lock(_mutex);
	_numThreads = numThreads;
}

string TableItem::GetNumThreads()
{
	scoped_lock(_mutex);
	return _numThreads;
}

