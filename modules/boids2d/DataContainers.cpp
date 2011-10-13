/*
 * DataContainers.cpp
 *
 *  Created on: Jun 5, 2011
 *      Author: arild
 */
#include <sstream>

#include "DataContainers.h"

BoidsApp  *VisualBase::boidsApp;
Table *VisualBase::table;

VisualBase::VisualBase()
{
}

VisualBase::~VisualBase()
{
	delete cpu;
	delete memory;
	delete network;
	delete tableItem;
}

void VisualBase::InitBoids(ProcessMessage &msg)
{
	int red, green, blue;
	Data::NameToRgbColor(msg.processname(), &red, &green, &blue);

	cpu = boidsApp->CreateBoid(new BoidSharedContext(red, green, blue, QUAD));
	memory = boidsApp->CreateBoid(new BoidSharedContext(red, green, blue, TRIANGLE_UP));
	network = boidsApp->CreateBoid(new BoidSharedContext(red, green, blue, DIAMOND));
	storage = boidsApp->CreateBoid(new BoidSharedContext(red, green, blue, POLYGON));

	// Put the boids contexts into a table that presents them visually
	// in another way, and allows for events
	tableItem = new TableItem(msg.processname());
	tableItem->AddBoid(cpu->ctx);
	tableItem->AddBoid(memory->ctx);
	tableItem->AddBoid(network->ctx);
	tableItem->AddBoid(storage->ctx);
	table->Add(tableItem);
}

StatBase::StatBase()
{
	userCpuUtilization = 0;
	systemCpuUtilization = 0;
	memoryUtilization = 0;
	ioInUtilization = 0;
	ioOutUtilization = 0;
	networkInUtilization = 0;
	networkOutUtilization = 0;
	storageInUtilization = 0;
	storageOutUtilization = 0;

	userCpuUtilizationSum = 0;
	systemCpuUtilizationSum = 0;
	memoryUtilizationSum = 0;
	ioInUtilizationSum = 0;
	ioOutUtilizationSum = 0;

	numSamples = 0;
}

ProcStat::ProcStat()
{
}

ProcVisual::ProcVisual(ProcessMessage &msg)
{
	InitBoids(msg);
}

ProcVisual::~ProcVisual()
{
}

Proc::Proc(ProcessMessage &msg)
{
	stat = new ProcStat();
	visual = new ProcVisual(msg);
}

Proc::~Proc()
{

}


ProcNameStat::ProcNameStat()
{
}

ProcNameStat::~ProcNameStat()
{
}

ProcNameVisual::ProcNameVisual(ProcessMessage &msg)
{
	InitBoids(msg);
	VisualBase::table->Add(tableItem);
}

ProcNameVisual::~ProcNameVisual()
{

}

ProcName::ProcName(ProcessMessage &msg)
{
	stat = new ProcNameStat();
	visual = new ProcNameVisual(msg);
}

ProcName::~ProcName()
{
	delete stat;
	delete visual;
}

NodeStat::NodeStat()
{
}

NodeStat::~NodeStat()
{
}

NodeVisual::NodeVisual()
{
}

NodeVisual::~NodeVisual()
{
}

Node::Node()
{
}

Node::~Node()
{
}

DataUpdate::DataUpdate()
{
}

Data::Data()
{
}

Data::~Data()
{
}

Proc *Data::Update(ProcessMessage &msg)
{
	string procMapKey = _CreateProcKey(msg.hostname(), msg.pid());
	if (msg.isterminated()) {
		if (procMap.count(procMapKey) > 0) {
			Proc *proc = procMap[procMapKey];
			procMap.erase(procMapKey);
			BoidsApp *app = proc->visual->boidsApp;
			app->RemoveBoid(proc->visual->cpu);
			app->RemoveBoid(proc->visual->memory);
			app->RemoveBoid(proc->visual->network);
			app->RemoveBoid(proc->visual->storage);
			proc->visual->table->Remove(proc->visual->tableItem);
		}

		return NULL;
	}

	Proc *proc;
	if (procMap.count(procMapKey) == 0) {
		proc = new Proc(msg);
		procMap[procMapKey] = proc;
	}
	else
		proc = procMap[procMapKey];
	return proc;
}

string Data::_CreateProcKey(string hostName, int pid)
{
	stringstream ss;
	ss << hostName << pid;
	return ss.str();
}

void Data::NameToRgbColor(string name, int *r, int *g, int *b)
{
	unsigned long hash = _OatHash(name);
	*r = hash & 0xFF;
	*g = (hash >> 8) & 0xFF;
	*b = (hash >> 16) & 0xFF;
	// Make sure color does not become too dark
	if (*r < 20)
		*r = 20;
	if (*g < 20)
		*g = 20;
	if (*b < 20)
		*b = 20;
}

/**
 * One-at-a-time hash by Bob Jenkins: http://en.wikipedia.org/wiki/Jenkins_hash_function
 *
 * This hash function appears to have sufficient avalanch effect, especially compared
 * to the hash function found in boost (<boost/functional/hash.hpp>).
 */
unsigned long Data::_OatHash(string key)
{
	unsigned char *p = (unsigned char *)key.c_str();
	unsigned long h = 0;
	int i;

	for (i = 0; i < key.length(); i++) {
		h += p[i];
		h += (h << 10);
		h ^= (h >> 6);
	}

	h += (h << 3);
	h ^= (h >> 11);
	h += (h << 15);

	return h;
}





















