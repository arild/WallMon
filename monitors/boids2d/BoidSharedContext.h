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
	static int tailLength;

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
