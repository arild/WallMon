/*
 * WallView.cpp
 *
 *  Created on: Apr 22, 2011
 *      Author: arild
 */

#include "System.h"
#include "WallView.h"

#define WALL_WIDHT		7
#define WALL_HEIGHT		4

#define SCREEN_WIDTH	1024
#define SCREEN_HEIGHT	768

#include <iostream>
using namespace std;

// 1D array representing a bottom-up row-wise representation of the display wall
string HOSTNAMES[WALL_WIDHT * WALL_HEIGHT] = { "tile-0-0", "tile-1-0", "tile-2-0", "tile-3-0",
		"tile-4-0", "tile-5-0", "tile-6-0", "tile-0-1", "tile-1-1", "tile-2-1", "tile-3-1",
		"tile-4-1", "tile-5-1", "tile-6-1", "tile-0-2", "tile-1-2", "tile-2-2", "tile-3-2",
		"tile-4-2", "tile-5-2", "tile-6-2", "tile-0-3", "tile-1-3", "tile-2-3", "tile-3-3",
		"tile-4-3", "tile-5-3", "tile-6-3" };

WallView::WallView(int x, int y, int width, int height)
{
	_x = x;
	_y = y;
	_w = width;
	_h = height;
	_hostname = System::GetHostname();

	// Traverse row-wise
	for (int i = 0; i < _y + _h && i < WALL_HEIGHT; i++) {
		for (int j = 0; j < _x + _w && j < WALL_WIDHT; j++) {
			string tileHostname = HOSTNAMES[j + (i * WALL_WIDHT)];
			_grid.push_back(tileHostname);
		}
	}
}

vector<string> WallView::Get()
{
	return _grid;
}

bool WallView::IsWithin()
{
	if (_GetIndex(_hostname) == -1)
		return false;
	return true;
}

void WallView::GetOrientation(double *x, double *y, double *width, double *height)
{
	GetOrientation(x, y, width, height, _hostname);
}

void WallView::GetOrientation(double *x, double *y, double *width, double *height, string hostname)
{
	int posx, posy, index;
	index = _GetIndex(hostname);
	cout << "Idx: " << index << endl;
	_IndexToCoordinates(index, &posx, &posy);
	cout << "Posx: " << posx << endl;
	cout << "Posy: " << posy << endl;
	*x = (posx / (double)_w) * 100.;
	*y = (posy / (double)_h) * 100.;
	*width = (1 / (double)_w) * 100.;
	*height = (1 / (double)_h) * 100.;
}

int WallView::_GetIndex(string hostname)
{
	for (int i = 0; i < _grid.size(); i++) {
		if (_grid[i].compare(0, 8, hostname, 0, 8) == 0)
			return i;
	}
	return -1;
}

void WallView::_IndexToCoordinates(int index, int *x, int *y)
{
	*x = index % _w;
	*y = index / _w;
}

