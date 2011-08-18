
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


const PLFLT BarChart::y0[10] = { 5., 15., 12., 24., 28., 30., 20., 8., 12., 3. };

PLFLT BarChart::      pos[] = { 0.0, 0.25, 0.5, 0.75, 1.0 };
PLFLT BarChart::      red[] = { 0.0, 0.25, 0.5, 1.0, 1.0 };
PLFLT BarChart::      green[] = { 1.0, 0.5, 0.5, 0.5, 1.0 };
PLFLT BarChart::      blue[] = { 1.0, 1.0, 0.5, 0.25, 0.0 };

const char *argv = ". -geometry 1000x800";
int argc = 2;

BarChart::BarChart()
{
    _labelMap = *new LabelMap();
    _labelPos = 0;
    int  i;
    char string[20];
	//return;
    pls = new plstream();

    plsdev("xcairo"); // output device
    // Parse and process command line arguments.
    //pls->parseopts(&argc,&argv, PL_PARSE_FULL );

    // Initialize plplot.
    pls->init();

    pls->adv( 0 );
    pls->vsta(); // Selects standard viewport
    pls->wind( 0.0, 29., 0.0, 150.0 ); // Relative to points in plot
    //pls->box( "bc", 1.0, 0, "bcnv", 100.0, 0 );
    pls->box( "", 1.0, 0, "", 100.0, 0 );
    pls->col0( 2 );
    pls->lab( "Nodes", "CPU Usage", "" );
    pls->scmap1l( true, 5, pos, red, green, blue, NULL );
}

BarChart::~BarChart()
{
	delete pls;
}

void BarChart::_DrawBar( PLFLT x0, PLFLT y0 )
{
    PLFLT *x = new PLFLT[4];
    PLFLT *y = new PLFLT[4];

    x[0] = x0;
    y[0] = 0.;
    x[1] = x0;
    y[1] = y0;
    x[2] = x0 + 1.;
    y[2] = y0;
    x[3] = x0 + 1.;
    y[3] = 0.;

    pls->fill( 4, x, y );
    pls->col0( 1 );
    pls->lsty( 1 );
    pls->line( 4, x, y );

    delete[] x;
    delete[] y;
}

void BarChart::_ClearBar( PLFLT x0, PLFLT y0 )
{
    PLFLT *x = new PLFLT[4];
    PLFLT *y = new PLFLT[4];

    x[0] = x0;
    y[0] = 0.;
    x[1] = x0;
    y[1] = y0;
    x[2] = x0 + 1.;
    y[2] = y0;
    x[3] = x0 + 1.;
    y[3] = 0.;

    pls->col0( 0 );
    pls->fill( 4, x, y );
    delete[] x;
    delete[] y;
}

void BarChart::Update(string label, int value)
{
	if (_labelMap.count(label) == 0) {
		_labelMap[label] = _labelPos;
		_labelPos += 1;
	}

	int x = _labelMap[label];
	//pls->col1(0.5);
	//plscol0(0.5, 0, 0, 0);

	_ClearBar(x, 100);
	pls->col1( x / 29.0 );
	pls->psty( 0 );
	_DrawBar(x , value);
	char valueLabel[100];
	sprintf( valueLabel, "%d", value );
	pls->ptex( ( x + .5 ), (value + 3. ), 1.0, 0.0, .5, valueLabel);

	// Label below each bar
	pls->mtex( "tv", 1.0, ( ( x + 0.5 ) * .1 ), .5, label.c_str());

	pls->flush();
	//LOG(INFO) << "Update: Key: " << label << " : value: " << value;
}




