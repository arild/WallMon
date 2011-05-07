/*
 * PidMonitor.cpp
 *
 *  Created on: Apr 16, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include <algorithm>
#include "System.h"
#include "PidMonitor.h"

int isSorted_(vector<int> *v)
{
	for (int i = 0; i < v->size() - 1; i++)
		if ((*v)[i] > (*v)[i + 1])
			return 0;
	return 1;
}

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

vector<int> PidMonitor::GetUnion()
{
	vector<int> res(_new->size() + _old->size());
	vector<int>::iterator it = set_union(_new->begin(), _new->end(), _old->begin(), _old->end(),
			res.begin());
	res.erase(it, res.end());
	return res;
}

vector<int> PidMonitor::GetIntersection()
{
	int size = std::min(_new->size(), _old->size());
	vector<int> res(size);
	vector<int>::iterator it = set_intersection(_new->begin(), _new->end(), _old->begin(),
			_old->end(), res.begin());
	res.erase(it, res.end());
	_RemoveIgnored(res);
	return res;
}

vector<int> PidMonitor::GetDifference()
{
	vector<int> res(_new->size());
	vector<int>::iterator it = set_difference(_new->begin(), _new->end(), _old->begin(),
			_old->end(), res.begin());
	res.erase(it, res.end());
	return res;
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

