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

using namespace std;


class WallView {
public:
	WallView(int x, int y, int width, int height);
	vector<string> Get();
	bool IsWithin();
	void GetOrientation(double *x, double *y, double *width, double *height);
	void GetOrientation(double *x, double *y, double *width, double *height, string hostname);
private:
	int _x, _y, _w, _h;
	string _hostname;
	vector<string> _grid;
	int _GetIndex(string hostname);
	void _IndexToCoordinates(int index, int *x, int *y);
};

#endif /* WALLVIEW_H_ */
