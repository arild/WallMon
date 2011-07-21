/*
 * WallView.cpp
 *
 *  Created on: Apr 22, 2011
 *      Author: arild
 */

#include "System.h"
#include "WallView.h"

#include <iostream>
using namespace std;

// 1D array representing a bottom-up row-wise representation of the display wall
string HOSTNAMES[WALL_WIDHT * WALL_HEIGHT] = { "tile-0-0"};//, "tile-1-0", "tile-2-0", "tile-3-0",
//		"tile-4-0", "tile-5-0", "tile-6-0", "tile-0-1", "tile-1-1", "tile-2-1", "tile-3-1",
//		"tile-4-1", "tile-5-1", "tile-6-1", "tile-0-2", "tile-1-2", "tile-2-2", "tile-3-2",
//		"tile-4-2", "tile-5-2", "tile-6-2", "tile-0-3", "tile-1-3", "tile-2-3", "tile-3-3",
//		"tile-4-3", "tile-5-3", "tile-6-3" };

WallView::WallView(int x, int y, int width, int height)
{
	_x = x;
	_y = y;
	_w = width;
	_h = height;
	_hostname = System::GetHostname();

	// Traverse row-wise
	for (int i = _y; i < _y + _h && i < WALL_HEIGHT; i++) {
		for (int j = _x; j < _x + _w && j < WALL_WIDHT; j++) {
			string tileHostname = HOSTNAMES[j + (i * WALL_WIDHT)];
			_grid.push_back(tileHostname);
		}
	}
}

/**
 * Returns the 1D bottom-up row-wise array of tile names generated in the constructor
 */
vector<string> WallView::GetGrid()
{
	return _grid;
}

/**
 * Determines if a tile is within the grid defined in the constructor
 */
bool WallView::IsTileWithin()
{
	if (_GetIndex(_hostname) == -1)
		return false;
	return true;
}

/**
 * Determines if given global coordinates are within the defined grid
 */
bool WallView::IsCordsWithin(float x, float y)
{
	if (x >= _x * TILE_SCREEN_WIDTH && x <= (_x + _w) * TILE_SCREEN_WIDTH && y >= _y
			* TILE_SCREEN_HEIGHT && y <= (_y + _h) * TILE_SCREEN_HEIGHT)
		return true;
	return false;
}

/**
 * Translate given global coordinates to 'global coordinates' within given grid
 *
 * This method is target for systems that internally operate on a coordinate system
 * that exactly matches the  resolution of the display wall. This kind of abstraction
 * is often convenient when developing for the display wall.
 */
void WallView::GlobalToGridCoords(float *x, float *y)
{
	float localX = *x - (TILE_SCREEN_WIDTH * _x);
	float localY = *y - (TILE_SCREEN_HEIGHT * _y);
	*x = localX * (WALL_WIDHT / (float)_w);
	*y = localY * (WALL_HEIGHT / (float)_h);
}

/**
 * Calculates the display area for the given tile
 *
 * The display area is based on the grid defined in the constructor and (always)
 * the resolution of the entire display wall, 7168x3072. This method is intended
 * for unit-testing due to the host name parameter, otherwise use the one without.
 */
void WallView::GetDisplayArea(double *x, double *y, double *width, double *height, string hostname)
{
	int posx, posy, index;
	index = _GetIndex(hostname);
	_IndexToCoordinates(index, &posx, &posy);
	*x = (posx / (double) _w) * (double) WALL_SCREEN_WIDTH;
	*y = (posy / (double) _h) * (double) WALL_SCREEN_HEIGHT;
	*width = (1 / (double) _w) * (double) WALL_SCREEN_WIDTH;
	*height = (1 / (double) _h) * (double) WALL_SCREEN_HEIGHT;
}

void WallView::GetDisplayArea(double *x, double *y, double *width, double *height)
{
	GetDisplayArea(x, y, width, height, _hostname);
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

