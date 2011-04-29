/*
 * BoidSharedContext.h
 *
 *  Created on: Apr 10, 2011
 *      Author: arild
 */

#ifndef BOIDSHAREDCONTEXT_H_
#define BOIDSHAREDCONTEXT_H_

#include <boost/thread/mutex.hpp>

using namespace std;

typedef boost::mutex::scoped_lock scoped_lock;

enum BoidShape {
	QUAD,
	TRIANGLE,
	CIRCLE,
	DIAMOND
};

class BoidSharedContext {
public:
	int red, green, blue;
	BoidShape shape;

	void SetDestination(float x, float y);
	void GetDestination(float *x, float *y);

private:
	float _destx, _desty;
	boost::mutex _mutex;
};
#endif /* BOIDSHAREDCONTEXT_H_ */
