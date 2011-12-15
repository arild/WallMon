/**
 * Helper class for organizing tiles on the display wall.
 *
 * This class provides an abstraction where a grid of tiles (on the display wall)
 * is defined. Several operations can be carried out on this grid, such as determining
 * if a given tile or pair of coordinates is within the grid.
 *
 * IMPORTANT: After a grid has been defined, some of the methods assumes that the
 * virtual resolution within this grid is mapped 1:1. For example, given a 2x2 grid
 * with a total physical resolution of 2048x1536, the virtual resolution is the same.
 * A previous approach to this was to always have the entire display wall resolution,
 * 7168x3072 as the virtual resolution, which was convenient in some cases. However,
 * when scaling down, to for example 2x2 tiles, it was hard to predict the final visual result.
 */

#include "System.h"
#include "WallView.h"
#include <map>
#include <boost/tuple/tuple.hpp>
#include <boost/assign.hpp>

using namespace std;
using namespace boost::tuples;
using namespace boost::assign;

// 1D array representing a bottom-up row-wise representation of the display wall
string HOSTNAMES[WALL_WIDHT * WALL_HEIGHT] = { "tile-0-0", "tile-1-0", "tile-2-0", "tile-3-0",
		"tile-4-0", "tile-5-0", "tile-6-0", "tile-0-1", "tile-1-1", "tile-2-1", "tile-3-1",
		"tile-4-1", "tile-5-1", "tile-6-1", "tile-0-2", "tile-1-2", "tile-2-2", "tile-3-2",
		"tile-4-2", "tile-5-2", "tile-6-2", "tile-0-3", "tile-1-3", "tile-2-3", "tile-3-3",
		"tile-4-3", "tile-5-3", "tile-6-3" };

map<string, string> ipAddresses = map_list_of("tile-0-0", "10.1.255.238")("tile-0-1",
		"10.1.255.237")("tile-0-2", "10.1.255.236")("tile-0-3", "10.1.255.234")("tile-1-0",
		"10.1.255.233")("tile-1-1", "10.1.255.232")("tile-1-2", "10.1.255.231")("tile-1-3",
		"10.1.255.230")("tile-2-0", "10.1.255.229")("tile-2-1", "10.1.255.228")("tile-2-2",
		"10.1.255.227")("tile-2-3", "10.1.255.226")("tile-3-0", "10.1.255.225")("tile-3-1",
		"10.1.255.224")("tile-3-2", "10.1.255.223")("tile-3-3", "10.1.255.222")("tile-4-0",
		"10.1.255.221")("tile-4-1", "10.1.255.220")("tile-4-2", "10.1.255.219")("tile-4-3",
		"10.1.255.218")("tile-5-0", "10.1.255.217")("tile-5-1", "10.1.255.216")("tile-5-2",
		"10.1.255.215")("tile-5-3", "10.1.255.214")("tile-6-0", "10.1.255.213")("tile-6-1",
		"10.1.255.212")("tile-6-2", "10.1.255.211")("tile-6-3", "10.1.255.210");
/**
 * Defines a grid on the display wall
 *
 * Later operations in this instance will take this grid into account
 */
WallView::WallView(int x, int y, int width, int height, string hostname)
{
	_x = x;
	_y = y;
	_w = width;
	_h = height;

	_hostname = hostname;
	if (_hostname.empty())
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
 *
 * This might be useful if the caller needs all host names of the current grid
 */
vector<string> WallView::GetGrid()
{
	return _grid;
}

vector<string> WallView::GetGridIpAddress()
{
	vector<string> ret;
	vector<string> grid = GetGrid();
	for (int i = 0; i < grid.size(); i++)
		ret.push_back(ipAddresses[grid[i]]);
	return ret;
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
 * Determines if a tile is at the lower left position of the grid.
 *
 * This method is intended to be used to select a master within the grid.
 */
bool WallView::IsLowerLeft()
{
	if (_GetIndex(_hostname) == 0)
		return true;
	return false;
}

/**
 * Determines if given global coordinates are within the defined grid
 *
 * This method is intended to be used in collaboration with the shout event system
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
	*x = *x - (TILE_SCREEN_WIDTH * _x);
	*y = *y - (TILE_SCREEN_HEIGHT * _y);
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
	*x = (posx / (double) _w) * (double) (TILE_SCREEN_WIDTH * _w);
	*y = (posy / (double) _h) * (double) (TILE_SCREEN_HEIGHT * _h);
	*width = TILE_SCREEN_WIDTH;
	*height = TILE_SCREEN_HEIGHT;
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

/**
 * Returns the total width of the grid in pixels
 */
int WallView::GetTotalPixelWidth()
{
	return TILE_SCREEN_WIDTH * _w;
}

/**
 * Returns the total height of the grid in pixels
 */
int WallView::GetTotalPixelHeight()
{
	return TILE_SCREEN_HEIGHT * _h;
}

int WallView::GetWidth()
{
	return _w;
}

int WallView::GetHeight()
{
	return _h;
}
