/*	WVGeometry.cxx
	(c) 2008-2010 Daniel Stodle, daniels@cs.uit.no
*/

#include	"WVGeometry.h"
#include	"shout/event-types/line_intersect.h"
#include	<string>

WVRect	WVRect::intersect(const WVRect &r) {
	WVRect		isect = WVRect(-1., -1., 0., 0.);
	GLFloatType	right1, right2,
				bottom1, bottom2;
	
	right1	= origin.x + size.x;
	right2	= r.origin.x + r.size.x;
	bottom1	= origin.y + size.y;
	bottom2	= r.origin.y + r.size.y;
	//	Intersect vertically
	if (origin.x <= r.origin.x && right1 >= r.origin.x) {
		isect.origin.x	= r.origin.x;
		if (right1 > right2)
			isect.size.x	= r.size.x;
		else
			isect.size.x	= right1 - isect.origin.x;
	}
	else if (r.origin.x < origin.x && right2 > origin.x) {
		isect.origin.x	= origin.x;
		if (right2 > right1)
			isect.size.x	= size.x;
		else
			isect.size.x	= right2 - isect.origin.x;
	}
	//	Intersect horizontally
	if (origin.y <= r.origin.y && bottom1 >= r.origin.y) {
		isect.origin.y	= r.origin.y;
		if (bottom1 > bottom2)
			isect.size.y	= r.size.y;
		else
			isect.size.y	= bottom1 - isect.origin.y;
	}
	else if (r.origin.y < origin.y && bottom2 > origin.y) {
		isect.origin.y	= origin.y;
		if (bottom2 > bottom1)
			isect.size.y	= size.y;
		else
			isect.size.y	= bottom2 - isect.origin.y;
	}
	return isect;
}


bool	WVRect::intersects(const WVRect &r) {
	WVRect	isect = intersect(r);
	return !isect.empty();
}


WVRect	WVRect::rect_union(const WVRect &r) {
	WVRect	result;
	
	result.origin.x	= std::min(origin.x, r.origin.x);
	result.origin.y	= std::min(origin.y, r.origin.y);
	result.size.x	= std::max((origin.x+size.x)-result.origin.x, (r.origin.x+r.size.x)-result.origin.x);
	result.size.y	= std::max((origin.y+size.y)-result.origin.y, (r.origin.y+r.size.y)-result.origin.y);
	return result;
}


int				WVRect::subtract(WVRect &sub, WVRect *sub_rect /* pre-allocated array of 4 */) {
	int				num_new	= 8;
	//WVRect			sub_rect[8],
	//				joined;
	GLFloatType		x[4], y[4];
	int				i, 
					/*	The map-array defines the top-left and bottom-right
						coordinates of the 8 rectangles as indices into the x- and y-
						arrays. Each element of map[i] = [tl.x, tl.y, br.x, br.y]
						The conceptual rect looks like this:
						
					X:	0   1    2   3	Y:
						+---+----+---+	0
						| 0 * 1  * 2 |
						+...+----+...+	1
						| 3 |sub | 4 |
						+...+----+...+	2
						| 5 * 6  * 7 |
						+---+----+---+	3
					*/			//	  x  y  x  y
					map[8][4]	= { { 0, 0, 1, 1 },	//	top-left
									{ 1, 0, 2, 1 },	//	top-center
									{ 2, 0, 3, 1 },	//	top-right
									{ 0, 1, 1, 2 },	//	center-left
									{ 2, 1, 3, 2 },	//	center-right
									{ 0, 2, 1, 3 },	//	bottom-left,
									{ 1, 2, 2, 3 },	//	bottom-center,
									{ 2, 2, 3, 3 }	//	bottom-right,
								  };
	x[0]	= std::min(origin.x, sub.x());
	x[1]	= std::max(origin.x, sub.x());
	x[2]	= std::min(origin.x+size.x, sub.x()+sub.width());
	x[3]	= std::max(origin.x+size.x, sub.x()+sub.width());
	y[0]	= std::min(origin.y, sub.y());
	y[1]	= std::max(origin.y, sub.y());
	y[2]	= std::min(origin.y+size.y, sub.y()+sub.height());
	y[3]	= std::max(origin.y+size.y, sub.y()+sub.height());
	for (i=0;i<8;i++) {
		sub_rect[i]		= WVRect(x[map[i][0]], y[map[i][1]], x[map[i][2]]-x[map[i][0]], y[map[i][3]]-y[map[i][1]]);
		if (sub_rect[i].empty()) {
			num_new--;
			printf("Rect %d empty: ", i);
			sub_rect[i].describe();
		}
	}
	//	Join rects, start with top row
/*	joined	= sub_rect[0].rect_union(sub_rect[1]);
	joined	= joined.rect_union(sub_rect[2]);
	if (!joined.empty())
		res[num_new++]	= joined;
	
	//	Center-left and center-right can't be joined..
	if (!sub_rect[3].empty())
		res[num_new++]	= sub_rect[3];
	if (!sub_rect[4].empty())
		res[num_new++]	= sub_rect[4];
	
	//	Finally, join bottom row
	joined	= sub_rect[5].rect_union(sub_rect[6]);
	joined	= joined.rect_union(sub_rect[7]);
	if (!joined.empty())
		res[num_new++]	= joined;
*/	
	return num_new;
}


void	WVPoint3d::wrap(GLFloatType min_lim, GLFloatType max_lim) {
	GLFloatType	*values[3]	= { &x, &y, &z }, dist = max_lim-min_lim;
	int		i;
	for (i=0;i<3;i++) {
		if (*values[i] < min_lim)
			*values[i]	+= dist;
		if (*values[i] > max_lim)
			*values[i]	-= dist;
	}
}



WVPoint2d	WVLine::intersect(const WVLine &other, bool *valid) {
	double	p[8] = { pts[0].x, pts[0].y, pts[1].x, pts[1].y,  other.pts[0].x, other.pts[0].y, other.pts[1].x, other.pts[1].y };
	double	rx = 0., ry = 0.;
	int		res;
	res	= line_intersect(p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], &rx, &ry);
	if (valid)
		*valid	= res;
	return WVPoint2d(rx, ry);
}



WVCircle	WVCircle::circle_union(const WVCircle &c) {
	if (contains(c))
		return WVCircle(origin, radius);
	else if (origin.distance(c.origin)+radius < c.radius)
		return WVCircle(c.origin, c.radius);
	WVPoint2d	center	= (origin+c.origin)*0.5;
	GLFloatType	newRad	= origin.distance(c.origin)+(radius*0.5)+(c.radius*0.5);
	return WVCircle(center, newRad);
}



bool	WVCircle::contains(const WVCircle &c) {
	if (origin.distance(c.origin)+c.radius < radius)
		return true;
	return false;
}
