/*
 * Plot.h
 *
 *  Created on: Oct 31, 2011
 *      Author: arild
 */

#ifndef PLOT_H_
#define PLOT_H_

#include <string>
#include "Stat.h"
#include "gnuplot_i.hpp"

using namespace std;

class Plot {
public:
	Plot();
	virtual ~Plot();
	static void GenerateVisualizationEnginePlot(vector<Stat<double> >);
private:
	Gnuplot _plot;

};

#endif /* PLOT_H_ */
