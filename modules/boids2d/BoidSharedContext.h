/*
 * BoidSharedContext.h
 *
 *  Created on: Apr 10, 2011
 *      Author: arild
 */

#ifndef BOIDSHAREDCONTEXT_H_
#define BOIDSHAREDCONTEXT_H_

#include <boost/thread/mutex.hpp>
#include "Entity.h"

using namespace std;

typedef boost::mutex::scoped_lock scoped_lock;

enum BoidType {
	BOID_TYPE_CORE,
	BOID_TYPE_PROCESS,
	BOID_TYPE_PROCESS_NAME,
	BOID_TYPE_NODE
};

class BoidDisplayText {
public:
	string procName, hostName;
};

class BoidSharedContext {
public:
	int red, green, blue;
	Shape boidShape;
	BoidType boidType;
	BoidDisplayText displayText;
	static int tailLength;

	// Which type of boids to show within a metric
	static bool showCpuBoid, showMemoryBoid, showNetworkBoid, showStorageBoid;

	BoidSharedContext(int red_, int green_, int blue_, Shape shape_, BoidType boidType_);

	void SetDestination(float x, float y);
	void GetDestination(float *x, float *y);

	void SetTailLength(int length);
	int GetTailLength();

	void ShowDisplayText();
	void DisableDisplayText();
private:
	float _destx, _desty;
	int _tailLength;
	bool _showDisplayText;
	boost::mutex _mutex;
};
#endif /* BOIDSHAREDCONTEXT_H_ */
