/*
 * DataContainers.h
 *
 *  Created on: Jun 5, 2011
 *      Author: arild
 */

#include <map>
#include <list>
#include <vector>
#include <string>
#include <boost/tuple/tuple.hpp>
#include "BoidSharedContext.h"
#include "NameTable.h"
#include "BoidsApp.h"


#ifndef DATACONTAINERS_H_
#define DATACONTAINERS_H_

using namespace std;
using namespace boost::tuples;
//using namespace ProcessMessage;


/**
 * Base class for visualizing a boid representing anything,
 * including a process, a process name, a node, a core, etc.
 */
class VisualBase {
public:
	int red, green, blue;
	BoidSharedContext *cpu;
	BoidSharedContext *memory;
	BoidSharedContext *network;
	TableItem *tableItem;

	static BoidsApp *boidsApp;
	static NameTable *nameTable;

	VisualBase();
	~VisualBase();
	void InitBoids(string identifierString, BoidType boidType);
};

/**
 * Base class for statistics associated with a boid
 */
class StatBase {
public:
	double userCpuUtilization;
	double systemCpuUtilization;
	double memoryUtilization;
	double networkInUtilization;
	double networkOutUtilization;
	double storageReadUtilization;
	double storageWriteUtilization;

	double userCpuUtilizationSum;
	double systemCpuUtilizationSum;
	double memoryUtilizationSum;
	double networkInUtilizationSum;
	double networkOutUtilizationSum;
	double storageReadUtilizationSum;
	double storageWriteUtilizationSum;

	unsigned int numSamples;
	StatBase();
	virtual ~StatBase() {}
};

class ProcStat : public StatBase {
public:
	ProcStat();
};

class ProcVisual : public VisualBase {
public:
	ProcVisual(string processName);
	~ProcVisual();
};

class Proc {
public:
	ProcStat *stat;
	ProcVisual *visual;

	Proc(string processName);
	~Proc();
};

typedef map<string, Proc *> ProcMap;


/**
 * Contains aggregated statistics about processes with the same name,
 * and has references to each process involved in the calculation.
 */
class ProcNameStat : public StatBase {
public:
	ProcNameStat();
	virtual ~ProcNameStat();
};

class ProcNameVisual : public VisualBase {
public:
	ProcNameVisual(string processName);
	virtual ~ProcNameVisual();
};

class ProcName {
public:
	ProcNameStat *stat;
	ProcNameVisual *visual;
	list<Proc *> procs;

	ProcName(string processName);
	virtual ~ProcName();
};
typedef map<string, ProcName *> ProcNameMap;


/**
 * Contains aggregated statistics about processes with the same name,
 * and has references to each process involved in the calculation.
 */
class NodeStat : StatBase {
public:
	NodeStat();
	virtual ~NodeStat();
};

class NodeVisual : VisualBase {
public:
	NodeVisual();
	virtual ~NodeVisual();
};

class Node {
public:
	NodeStat *stat;
	NodeVisual *visual;

	Node();
	virtual ~Node();
};
typedef map<string, Node *> NodeMap;


class DataUpdate {
public:
	Proc *proc;
	ProcName *procName;
	Node *node;

	bool procWasCreated;
	bool procNameWasCreated;
	bool nodeWasCreated;

	DataUpdate();
};

class Data {
public:
	ProcMap procMap;
	NodeMap nodeMap;
	ProcNameMap procNameMap;

	Data();
	~Data();
	DataUpdate Update(string hostname, string processName, int pid);
	static void NameToRgbColor(string name, float *r, float *g, float *b);
private:
	string _CreateProcKey(string hostName, int pid);
	static unsigned long _OatHash(string key);
};



#endif /* DATACONTAINERS_H_ */
