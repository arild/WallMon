/*
 * PidMonitor.cpp
 *
 *  Created on: Apr 16, 2011
 *      Author: arild
 */

#include "PidMonitor.h"

PidMonitor::PidMonitor()
{
	_pids = new set<int>();
}

PidMonitor::~PidMonitor()
{
	delete _pids;
}

void PidMonitor::Update()
{
}

set<int> PidMonitor::GetDiff()
{
}


void PidMonitor::Ignore(int pid)
{
}


