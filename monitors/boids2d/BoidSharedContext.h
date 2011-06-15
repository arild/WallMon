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

class BoidSharedContext {
public:
	int red, green, blue;
	Shape boidShape;
	BoidType boidType;
	static int tailLength;
	static BoidType boidTypeToShow; // Simple filter for the different metrics

	// Which type of boids to show within a metric
	static bool showCpuBoid, showMemoryBoid, showNetworkBoid, showStorageBoid;

	// Which type of metrics to show on the scrollable table
	static BoidType tableFrom; // Left one
	static BoidType tableTo; // Right one

	BoidSharedContext(int red_, int green_, int blue_, Shape shape_, BoidType boidType_);

	void SetDestination(float x, float y);
	void GetDestination(float *x, float *y);

	void SetTailLength(int length);
	int GetTailLength();

private:
	float _destx, _desty;
	int _tailLength;
	boost::mutex _mutex;
};
#endif /* BOIDSHAREDCONTEXT_H_ */
