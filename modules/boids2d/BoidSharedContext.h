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

class BoidSharedContext {
public:
	int red, green, blue;
	Shape boidShape;
	static bool useLogarithmicAxis;
	static float tailLength;

	// Which type of boids to show within a metric
	static bool showCpuBoid, showMemoryBoid, showNetworkBoid, showStorageBoid;

	BoidSharedContext(int red_, int green_, int blue_, Shape shape_);

	void SetDestination(float x, float y);
	void GetDestination(float *x, float *y);

	void SetTailLength(float length);
	float GetTailLength();

	void EnableHighlight(int highlightNumber);
	int GetHighlightNumber();
	void DisableHighlight();
	bool IsHighlighted();
private:
	float _destx, _desty;
	float _tailLength;
	bool _isHighlighted;
	int _highlightNumber;
	boost::mutex _mutex;
};
#endif /* BOIDSHAREDCONTEXT_H_ */
