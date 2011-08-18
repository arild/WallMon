/*
 * BarChart.h
 *
 *  Created on: Feb 24, 2011
 *      Author: arild
 */

#include "plc++demos.h"
#include <map>
#include <vector>

using namespace std;

class BarData {
public:
	string label;
	double user;
	double system;
	vector<double> vals;

	BarData(string _label, double _user, double _system)
	{
		label = _label;
		user = _user;
		system = _system;
	}

	BarData(){}

	vector<double> *GetValues()
	{
		vals.clear();
		if (user < 0.)
			user = 0.;
		if (system < 0.)
			system = 0.;
		vals.push_back(system);
		vals.push_back(user);
		return &vals;
	}
};

class BarChart {
public:
	BarChart(int numBars);
	virtual ~BarChart();
	void Render(double utilization, vector<BarData *> &barData);
private:
	void _DrawUtilization(PLFLT util);
	void _DrawBar(PLFLT barIndex, vector<double> &values);
	void _DrawLabel(PLFLT barIndex, string label);
	void _ClearAllBars();
	void _ClearAllLabels();
	void _ClearScreen();
	plstream *pls;
	plstream *frame;
	void _SelectTopWind();
	void _SelectMidWind();
	void _SelectBottomWind();
	PLFLT _window[4];
};
