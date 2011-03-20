#include <unistd.h>
//---------------------------------------------------------------------------//
// $Id: BarChart.cc 10785 2010-01-31 20:05:17Z hezekiahcarty $
//---------------------------------------------------------------------------//
//
//---------------------------------------------------------------------------//
// Copyright (C) 2004  Andrew Ross <andrewr@coriolis.greenend.org.uk>
// Copyright (C) 2004  Alan W. Irwin
//
// This file is part of PLplot.
//
// PLplot is free software; you can redistribute it and/or modify
// it under the terms of the GNU Library General Public License as published by
// the Free Software Foundation; version 2 of the License.
//
// PLplot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with PLplot; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
//---------------------------------------------------------------------------//
//
//---------------------------------------------------------------------------//
// Implementation of PLplot example 12 in C++.
//---------------------------------------------------------------------------//

#include <glog/logging.h>
#include "BarChart.h"
#ifdef PL_USE_NAMESPACE
using namespace std;
#endif

#define COLOR_BLACK		0
#define COLOR_RED		1
#define COLOR_YELLOW	2
#define COLOR_GREEN		3
#define COLOR_GREY		7
#define COLOR_BROWN		8

const char *argv = "-geometry 1000x600";
int argc = 1;

BarChart::BarChart(int numBars)
{
	_window[0] = _window[2] = 0.;
	_window[1] = (double)numBars;
	_window[3] = 105.;
	pls = new plstream();
	pls->parseopts(&argc, &argv, PL_PARSE_NOPROGRAM);
	//plsdev("xwin"); // output device
	pls->init();
	pls->adv(0); // Select the only sub-page present
	_SelectMidWind();
}

BarChart::~BarChart()
{
	delete pls;
}

void BarChart::Render(double utilization, vector<BarData *> &barData)
{
	_ClearScreen();
	for (int x = 0; x < barData.size(); x++) {
		pls->col1(x / _window[1]);
		_DrawUtilization(utilization);
		_DrawBar(x, *barData[x]->GetValues());
		_DrawLabel(x, barData[x]->label);
	}
	//_SelectBarWind();
	//pls->box( "bc", 1.0, 0, "bcnvt", 10.0, 0);
	pls->flush();
}

void BarChart::_DrawUtilization(PLFLT util)
{
	_SelectTopWind();
	char label[100];
	sprintf( label, "Total CPU Utilization: %.2f", util );
	PLFLT disp = 0.;
	PLFLT pos = 0.5;
	PLFLT just = 0.5;
	pls->mtex("b", disp, pos, just, label);
	//pls->ptex( 0., 0., 0., 0., .5, label);
}

void BarChart::_DrawBar(PLFLT barIndex, vector<double> &values)
{
	PLFLT valSum = 0.;
	int colors[2] = {COLOR_BROWN, COLOR_GREEN};
	_SelectMidWind();
	for (int i = 0; i < values.size(); i++) {
		PLFLT val = values[i];

		PLFLT x[4] = { barIndex, barIndex, barIndex + 1., barIndex + 1. };
		PLFLT y[4] = { valSum, valSum + val, valSum + val, valSum };

		pls->col0(colors[i % 2]);
		pls->fill(4, x, y);
		valSum += val;
	}

	PLFLT x[4] = { barIndex, barIndex, barIndex + 1., barIndex + 1. };
	PLFLT y[4] = { 0., valSum, valSum, 0.};
	pls->lsty(1);
	pls->col0(COLOR_RED);
	pls->line(4, x, y);

	char valueLabel[100];
	sprintf( valueLabel, "%.1f", valSum );
	pls->ptex( ( barIndex + .5 ), (valSum + 3. ), 1.0, 0.0, .5, valueLabel);
}

void BarChart::_DrawLabel(PLFLT barIndex, string label)
{
	_SelectBottomWind();
	PLFLT disp = -0.8;
	if ((int) barIndex % 2 == 0)
		disp = -2.4;
	double fraction = (1 / (double)_window[1]);
	PLFLT pos = (barIndex * fraction + (fraction / (double)2));
	PLFLT just = .5;

	if (label.length() > 10) {
		label.erase(11);
		label.replace(10, 1, ".");
	}

	pls->mtex("b", disp, pos, just, label.c_str());

	//	PLFLT x = barIndex;
	//	PLFLT y = height;
	//	PLFLT dx = barIndex + 1.;
	//	PLFLT dy = height;
	//	pls->ptex(x, y, dx, dy, 1., label.c_str());

}

void BarChart::_ClearAllBars()
{
	PLFLT *p = _window;
	PLFLT x[4] = { p[0], p[0], p[1], p[1] };
	PLFLT y[4] = { p[2], p[3], p[3], p[2] };

	_SelectMidWind();
	pls->col0(COLOR_BLACK);
	pls->fill(4, x, y);
}

void BarChart::_ClearAllLabels()
{
	PLFLT *p = _window;
	PLFLT x[4] = { p[0], p[0], p[1], p[1] };
	PLFLT y[4] = { p[2], p[3], p[3], p[2] };

	_SelectBottomWind();
	pls->col0(COLOR_BLACK);
	pls->fill(4, x, y);
}

void BarChart::_ClearScreen()
{
	pls->vpor(0., 1., 0., 1.);
	pls->wind(0., 1., 0., 1.); // Relative to points in plot
	PLFLT x[4] = { 0., 0., 1., 1. };
	PLFLT y[4] = { 0., 1., 1., 0. };

	pls->col0(COLOR_BLACK);
	pls->fill(4, x, y);
}

void BarChart::_SelectTopWind()
{
	double *p = _window;
	pls->vpor(0.1, 0.9, 0.95, 1.);
	pls->wind(p[0], p[1], p[2], p[3]); // Relative to points in plot
}


void BarChart::_SelectMidWind()
{
	double *p = _window;
	pls->vpor(0.1, 0.9, 0.2, 0.9);
	pls->wind(p[0], p[1], p[2], p[3]); // Relative to points in plot
}

void BarChart::_SelectBottomWind()
{
	double *p = _window;
	pls->vpor(0.1, 0.9, 0.1, 0.2);
	pls->wind(p[0], p[1], p[2], p[3]); // Relative to points in plot
}



