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

void VisualBase::InitBoids(string identifierString, BoidView boidType)
{
	float red, green, blue;
	Data::NameToRgbColor(identifierString, &red, &green, &blue);

	// Create contexts of boids - used to manage them
	cpu = new BoidSharedContext(red, green, blue, QUAD, boidType);
	memory = new BoidSharedContext(red, green, blue, TRIANGLE, boidType);
	network = new BoidSharedContext(red, green, blue, DIAMOND, boidType);

	// Put the boids contexts into a table that presents them visually
	// in another way, and allows for events
	tableItem = new TableItem(identifierString, red, green, blue);
	tableItem->AddBoid(cpu);
	tableItem->AddBoid(memory);
	tableItem->AddBoid(network);

	// Create the boids, and associated the boids contexts with them
	boidsApp->CreateBoid(cpu);
	boidsApp->CreateBoid(memory);
	boidsApp->CreateBoid(network);
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

	numSamples = 0;
}

ProcStat::ProcStat()
{
}

ProcVisual::ProcVisual(string processName)
{
	InitBoids(processName, BOID_TYPE_PROCESS);
}

ProcVisual::~ProcVisual()
{
}

Proc::Proc(string processName)
{
	stat = new ProcStat();
	visual = new ProcVisual(processName);
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

ProcNameVisual::ProcNameVisual(string processName)
{
	InitBoids(processName, BOID_TYPE_PROCESS_NAME);
	VisualBase::table->Add(tableItem);
}

ProcNameVisual::~ProcNameVisual()
{

}

ProcName::ProcName(string processName)
{
	stat = new ProcNameStat();
	visual = new ProcNameVisual(processName);
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
	procWasCreated = false;
	procNameWasCreated = false;
	nodeWasCreated = false;
}

Data::Data()
{
}

Data::~Data()
{
}

DataUpdate Data::Update(string hostname, string processName, int pid)
{
	DataUpdate update;

	// Per process
	string procMapKey = _CreateProcKey(hostname, pid);
	if (procMap.count(procMapKey) == 0) {
		update.proc = new Proc(processName);
		procMap[procMapKey] = update.proc;
		update.procWasCreated = true;
	}
	else
		update.proc = procMap[procMapKey];

	// Per process name
	if (procNameMap.count(processName) == 0) {
		update.procName = new ProcName(processName);
		procNameMap[processName] = update.procName;
		update.procNameWasCreated = true;
		update.procName->procs.push_back(update.proc);
	}
	else
		update.procName = procNameMap[processName];

	// Per node
//	if (nodeMap.count(hostname) == 0) {
//		update.node = new Node();
//		nodeMap[hostname] = update.node;
//		update.nodeWasCreated = true;
//	}
//	else
//		update.node = nodeMap[hostname];
//
//	if (update.procWasCreated)
//		update.node->procs.push_back(update.proc);

	return update;
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





















