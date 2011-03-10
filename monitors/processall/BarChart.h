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
	double value;

	BarData(string _label, double _value)
	{
		label = _label;
		value = _value;
	}
};

class BarChart {
public:
	BarChart();
	virtual ~BarChart();
	void Render(vector<BarData *> &values);
private:
	void _DrawBar(PLFLT barIndex, PLFLT value);
	void _DrawLabel( PLFLT barIndex, string label );
	void _ClearAllBars();
	void _ClearAllLabels();
	void _ClearScreen();
	plstream *pls;
	plstream *frame;
	static PLFLT pos[], red[], green[], blue[];
	void _SelectBarWind();
	void _SelectLabelWind();
};
