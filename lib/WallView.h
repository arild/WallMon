/*
 * WallView.h
 *
 *  Created on: Apr 22, 2011
 *      Author: arild
 */

#ifndef WALLVIEW_H_
#define WALLVIEW_H_

#include <vector>
#include <string>

#define TILE_SCREEN_WIDTH		1024
#define TILE_SCREEN_HEIGHT		768

#define WALL_WIDHT		7
#define WALL_HEIGHT		4

#define WALL_SCREEN_WIDTH		7168
#define WALL_SCREEN_HEIGHT		3072

using namespace std;

class WallView {
public:
	WallView(int x, int y, int width, int height);
	vector<string> Get();
	bool IsWithin();
	void GetOrientation(double *x, double *y, double *width, double *height, string hostname);
	void GetOrientation(double *x, double *y, double *width, double *height);
private:
	int _x, _y, _w, _h;
	string _hostname;
	vector<string> _grid;
	int _GetIndex(string hostname);
	void _IndexToCoordinates(int index, int *x, int *y);
};

#endif /* WALLVIEW_H_ */
