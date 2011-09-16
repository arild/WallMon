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

void VisualBase::InitBoids(ProcessMessage &msg, BoidView boidType)
{
	float red, green, blue;
	Data::NameToRgbColor(msg.processname(), &red, &green, &blue);

	// Create contexts of boids - used to manage them
	cpu = new BoidSharedContext(red, green, blue, QUAD, boidType);
	memory = new BoidSharedContext(red, green, blue, TRIANGLE, boidType);
	network = new BoidSharedContext(red, green, blue, DIAMOND, boidType);

	// Create the boids, and associated the boids contexts with them
	boidsApp->CreateBoid(cpu);
	boidsApp->CreateBoid(memory);
	boidsApp->CreateBoid(network);

	// Put the boids contexts into a table that presents them visually
	// in another way, and allows for events
	tableItem = new TableItem(msg.processname());
	tableItem->AddBoid(cpu);
	tableItem->AddBoid(memory);
	tableItem->AddBoid(network);
	table->Add(tableItem);
}

StatBase::StatBase()
{
	userCpuUtilization = 0;
	systemCpuUtilization = 0;
	memoryUtilization = 0;
	networkInUtilization = 0;
	networkOutUtilization = 0;
	storageReadUtilization = 0;
	storageWriteUtilization = 0;

	userCpuUtilizationSum = 0;
	systemCpuUtilizationSum = 0;
	memoryUtilizationSum = 0;
	networkInUtilizationSum = 0;
	networkOutUtilizationSum = 0;
	storageReadUtilizationSum = 0;
	storageWriteUtilizationSum = 0;

//	totalUserCpuUtilization = 0;
//	totalSystemCpuUtilization = 0;
//	totalMemoryUtilization = 0;
//	totalNetworkInUtilization = 0;
//	totalNetworkOutUtilization = 0;
//	totalStorageReadUtilization = 0;
//	totalStorageWriteUtilization = 0;

	numSamples = 0;
}

ProcStat::ProcStat()
{
}

ProcVisual::ProcVisual(ProcessMessage &msg)
{
	InitBoids(msg, BOID_TYPE_PROCESS);
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
	InitBoids(msg, BOID_TYPE_PROCESS_NAME);
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
		procMap.erase(procMapKey);
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

void Data::NameToRgbColor(string name, float *r, float *g, float *b)
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





















