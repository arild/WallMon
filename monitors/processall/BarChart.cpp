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

PLFLT BarChart::pos[] = { 0.0, 0.25, 0.5, 0.75, 1.0 };
PLFLT BarChart::red[] = { 0.0, 0.25, 0.5, 1.0, 1.0 };
PLFLT BarChart::green[] = { 1.0, 0.5, 0.5, 0.5, 1.0 };
PLFLT BarChart::blue[] = { 1.0, 1.0, 0.5, 0.25, 0.0 };

const char *argv = "-geometry 1000x600";
int argc = 1;

PLFLT BAR_WINDOW[4] = { 0., 10., 0., 100. };
PLFLT LABEL_WINDOW[4] = { 0., 10., 0., 100. };

BarChart::BarChart()
{
	pls = new plstream();
	pls->parseopts(&argc, &argv, PL_PARSE_NOPROGRAM);
	plsdev("xwin"); // output device
	pls->init();
	pls->adv(0); // Select the only sub-page present

	// "Sets up a standard viewport, leaving a left-hand margin of seven character heights, and four
	//character heights around the other three sides."
	//pls->vsta();
	_SelectBarWind();

	pls->col0(2);
	//pls->lab("Process Names", "CPU Usage", "");
	pls->scmap1l(true, 5, pos, red, green, blue, NULL);
	//pls->box("bcnst", 0.0, 0, "bcnstv", 0.0, 0);

	//frame = new plstream();
}

BarChart::~BarChart()
{
	delete pls;
}

void BarChart::Render(vector<BarData *> &barData)
{
	_ClearScreen();
	for (int x = 0; x < barData.size(); x++) {
		pls->col1(x / BAR_WINDOW[1]);
		_DrawBar(x, barData[x]->value);
		_DrawLabel(x, barData[x]->label);
	}
	pls->flush();
}

void BarChart::_DrawBar(PLFLT barIndex, PLFLT value)
{
	PLFLT x[4] = { barIndex, barIndex, barIndex + 1., barIndex + 1. };
	PLFLT y[4] = { 0., value, value, 0. };

	_SelectBarWind();
	pls->fill(4, x, y);
	pls->col0(1);
	pls->lsty(1);
	pls->line(4, x, y);

	char valueLabel[100];
	sprintf( valueLabel, "%.1f", value );
	pls->ptex( ( barIndex + .5 ), (value + 3. ), 1.0, 0.0, .5, valueLabel);
}

void BarChart::_DrawLabel(PLFLT barIndex, string label)
{
	_SelectLabelWind();
	PLFLT disp = -0.8;
	if ((int) barIndex % 2 == 0)
		disp = -2.4;
	PLFLT pos = (barIndex * .1 + .05);
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
	PLFLT *p = BAR_WINDOW;
	PLFLT x[4] = { p[0], p[0], p[1], p[1] };
	PLFLT y[4] = { p[2], p[3], p[3], p[2] };

	_SelectBarWind();
	pls->col0(0);
	pls->fill(4, x, y);
}

void BarChart::_ClearAllLabels()
{
	PLFLT *p = LABEL_WINDOW;
	PLFLT x[4] = { p[0], p[0], p[1], p[1] };
	PLFLT y[4] = { p[2], p[3], p[3], p[2] };

	_SelectLabelWind();
	pls->col0(0);
	pls->fill(4, x, y);
}

void BarChart::_ClearScreen()
{
	pls->vpor(0., 1., 0., 1.);
	pls->wind(0., 1., 0., 1.); // Relative to points in plot
	PLFLT x[4] = { 0., 0., 1., 1. };
	PLFLT y[4] = { 0., 1., 1., 0. };

	pls->col0(0);
	pls->fill(4, x, y);
}

void BarChart::_SelectBarWind()
{
	double *p = BAR_WINDOW;
	pls->vpor(0.1, 0.9, 0.2, 0.95);
	pls->wind(p[0], p[1], p[2], p[3]); // Relative to points in plot
}

void BarChart::_SelectLabelWind()
{
	double *p = LABEL_WINDOW;
	pls->vpor(0.1, 0.9, 0.1, 0.2);
	pls->wind(p[0], p[1], p[2], p[3]); // Relative to points in plot
}
