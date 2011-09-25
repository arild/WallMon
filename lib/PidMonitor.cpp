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
}

PidMonitor::~PidMonitor()
{
	delete _new;
	delete _old;
}

void PidMonitor::Update()
{
	_old->clear();
	vector<int> *tmp = _old;
	_old = _new;
	_new = tmp;

	System::GetAllPids(*_new);
	sort(_new->begin(), _new->end());
}

vector<int> PidMonitor::GetDifference()
{
	vector<int> ret(_new->size());
	vector<int>::iterator it = set_difference(_new->begin(), _new->end(), _old->begin(),
			_old->end(), ret.begin());
	ret.erase(it, ret.end());

	return ret;
}
