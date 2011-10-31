/*
 * Chart.h
 *
 *  Created on: Oct 25, 2011
 *      Author: arild
 */

#ifndef CHART_H_
#define CHART_H_

#include <vector>
#include <map>
#include "Stat.h"
#include "gnuplot_i.hpp"
#include "stubs/ProcessCollector.pb.h"

using namespace std;

typedef map<string, vector<ProcessMessage> *> DataContainer;

class Chart {
public:
	Chart();
	virtual ~Chart();
private:
	Gnuplot _plot;
	DataContainer _data;
};

#endif /* CHART_H_ */
