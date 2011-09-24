/*
 * PidMonitor.cpp
 *
 *  Created on: Apr 16, 2011
 *      Author: arild
 */

#include <algorithm>
#include "System.h"
#include "PidMonitor.h"

PidMonitor::PidMonitor()
{
	_new = new vector<int>;
	_old = new vector<int>;
	_ignored = new set<int> ;
}

PidMonitor::~PidMonitor()
{
	delete _new;
	delete _old;
	delete _ignored;
}

void PidMonitor::Update(vector<int> &pids)
{
	delete _old;
	_old = _new;
	_new = &pids;
	sort(_new->begin(), _new->end());
}

void PidMonitor::Update()
{
	Update(*System::GetAllPids());
}

vector<int> PidMonitor::GetDifference()
{
	vector<int> ret(_new->size());
	vector<int>::iterator it = set_difference(_new->begin(), _new->end(), _old->begin(),
			_old->end(), ret.begin());
	ret.erase(it, ret.end());
//	_RemoveIgnored(ret);
	return ret;
}

void PidMonitor::Ignore(int pid)
{
	_ignored->insert(pid);
}

void PidMonitor::_RemoveIgnored(vector<int> &pids)
{
	vector<int>::iterator it = set_difference(pids.begin(), pids.end(), _ignored->begin(),
			_ignored->end(), pids.begin());
	pids.erase(it, pids.end());
}

