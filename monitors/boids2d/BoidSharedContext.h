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
	PROCESS,
	PROCESS_NAME,
	NODE
};

class BoidSharedContext {
public:
	int red, green, blue;
	Shape boidShape;
	BoidType boidType;
	static int tailLength;
	static bool showCpuBoid, showMemoryBoid, showNetworkBoid, showStorageBoid;

	BoidSharedContext(int red_, int green_, int blue_, Shape shape_);

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
