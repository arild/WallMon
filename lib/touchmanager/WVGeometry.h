	/*	WVGeometry.h
	(c) 2008 Daniel Stodle, daniels@cs.uit.no
*/

#ifndef WVGEOMETRY_H
	#define WVGEOMETRY_H

//	Includes
#ifdef __APPLE_CC__
	#ifdef ITOUCH
		#include	<OpenGLES/ES2/gl.h>
		#include	<OpenGLES/ES2/glext.h>
	#else
		#include	<OpenGL/gl.h>
		#include	<OpenGL/glu.h>
		#include	<OpenGL/glext.h>
		#include	<GLUT/glut.h>
	#endif
#else
	#ifdef WANT_GLEW	
	#include	<GL/glew.h>
	#endif
	#include	<GL/gl.h>
	#include	<GL/glu.h>
	#include	<GL/glext.h>
#endif
#include	<stdio.h>
#include	<math.h>


#ifdef ITOUCH
typedef GLfloat GLFloatType;
#else
typedef GLdouble GLFloatType;
#endif

class WVPoint2d {
	public:
		WVPoint2d(void) : x(0.), y(0.) {};
		WVPoint2d(const WVPoint2d &pt) : x(pt.x), y(pt.y) {};
		WVPoint2d(GLFloatType ix, GLFloatType iy) : x(ix), y(iy) {};
		GLFloatType		x, y;
		
		void		translate(GLFloatType xx, GLFloatType yy) { x += xx; y += yy; };
		void		translate(const WVPoint2d &pt) { x += pt.x; y += pt.y; };
		GLFloatType	length() { return sqrt((x*x)+(y*y)); };
		GLFloatType	distance(const WVPoint2d &pt) { return sqrt(((pt.x-x)*(pt.x-x))+((pt.y-y)*(pt.y-y))); };
		void		limit(GLFloatType lim) { x = (x < -lim? -lim : (x > lim ? lim : x)); y = (y < -lim? -lim : (y>lim ? lim : y)); };
		//void		rotate(GLFloatType radians) { double dist = distance(WVPoint2d(0., 0.)); x += dist*cos(radians); y += dist*sin(radians); };
		
		
		WVPoint2d	operator+(const WVPoint2d &pt) { return WVPoint2d(x+pt.x,y+pt.y); };
		WVPoint2d&	operator+=(const WVPoint2d &pt) { x+=pt.x; y+=pt.y; return *this; };
		
		WVPoint2d	operator-(const WVPoint2d &pt) { return WVPoint2d(x-pt.x,y-pt.y); };
		WVPoint2d&	operator-=(const WVPoint2d &pt) { x-=pt.x; y-=pt.y; return *this; };
		
		WVPoint2d	operator*(const WVPoint2d &pt) { return WVPoint2d(x*pt.x,y*pt.y); };
		WVPoint2d&	operator*=(const WVPoint2d &pt) { x*=pt.x;y*=pt.y; return *this; };
		
		WVPoint2d	operator/(const WVPoint2d &pt) { return WVPoint2d(x/pt.x,y/pt.y); };
		WVPoint2d&	operator/=(const WVPoint2d &pt) { x/=pt.x;y/=pt.y; return *this; };
		
		WVPoint2d	operator+(const GLFloatType value) { return WVPoint2d(x+value,y+value); };
		WVPoint2d&	operator+=(const GLFloatType value) { x+=value; y+=value; return *this; };
		WVPoint2d	operator-(const GLFloatType value) { return WVPoint2d(x-value,y-value); };
		WVPoint2d&	operator-=(const GLFloatType value) { x-=value; y-=value; return *this; };
		WVPoint2d	operator*(const GLFloatType value) { return WVPoint2d(x*value,y*value); };
		WVPoint2d&	operator*=(const GLFloatType value) { x*=value; y*=value; return *this; };
		WVPoint2d	operator/(const GLFloatType value) { return WVPoint2d(x/value, y/value); };
		WVPoint2d&	operator/=(const GLFloatType value) { x/=value; y/=value; return *this; };
};

class WVPoint3d {
	public:
		WVPoint3d(void) : x(0.), y(0.), z(0.) {};
		WVPoint3d(const WVPoint2d &pt) : x(pt.x), y(pt.y), z(0.) {};
		WVPoint3d(const WVPoint3d &pt) : x(pt.x), y(pt.y), z(pt.z) {};
		WVPoint3d(GLFloatType ix, GLFloatType iy, GLFloatType iz) : x(ix), y(iy), z(iz) {};
		
		GLFloatType	x, y, z;
		
		void		translate(GLFloatType xx, GLFloatType yy, GLFloatType zz) { x += xx; y += yy; z += zz; };
		void		translate(const WVPoint3d &pt) { x += pt.x; y += pt.y; z += pt.z; };
		GLFloatType	distance(const WVPoint3d &pt) { return sqrt(((pt.x-x)*(pt.x-x))+((pt.y-y)*(pt.y-y))+((pt.z-z)*(pt.z-z))); };
		void		limit(GLFloatType lim) { x = (x < -lim? -lim : (x > lim ? lim : x)); y = (y < -lim? -lim : (y>lim ? lim : y)); z = (z < -lim? -lim : (z>lim ? lim : z)); };
		void		wrap(GLFloatType min_lim, GLFloatType WVx_lim);
		
		
		WVPoint3d	operator+(const WVPoint3d &pt) { return WVPoint3d(x+pt.x,y+pt.y,z+pt.z); };
		WVPoint3d&	operator+=(const WVPoint3d &pt) { x+=pt.x; y+=pt.y; z+=pt.z; return *this; };
		
		WVPoint3d	operator-(const WVPoint3d &pt) { return WVPoint3d(x-pt.x,y-pt.y,z-pt.z); };
		WVPoint3d&	operator-=(const WVPoint3d &pt) { x-=pt.x; y-=pt.y; z-=pt.z; return *this; };
		
		WVPoint3d	operator*(const WVPoint3d &pt) { return WVPoint3d(x*pt.x,y*pt.y,z*pt.z); };
		WVPoint3d&	operator*=(const WVPoint3d &pt) { x*=pt.x;y*=pt.y; z*=pt.z; return *this; };
		
		WVPoint3d	operator/(const WVPoint3d &pt) { return WVPoint3d(x/pt.x,y/pt.y, z/pt.z); };
		WVPoint3d&	operator/=(const WVPoint3d &pt) { x/=pt.x;y/=pt.y; z/=pt.z; return *this; };
		
		WVPoint3d	operator+(const GLFloatType value) { return WVPoint3d(x+value,y+value, z+value); };
		WVPoint3d&	operator+=(const GLFloatType value) { x+=value; y+=value; z+=value; return *this; };
		WVPoint3d	operator-(const GLFloatType value) { return WVPoint3d(x-value,y-value,z-value); };
		WVPoint3d&	operator-=(const GLFloatType value) { x-=value; y-=value; z-=value; return *this; };
		WVPoint3d	operator*(const GLFloatType value) { return WVPoint3d(x*value,y*value, z*value); };
		WVPoint3d&	operator*=(const GLFloatType value) { x*=value; y*=value; z*=value; return *this; };
		WVPoint3d	operator/(const GLFloatType value) { return WVPoint3d(x/value, y/value, z/value); };
		WVPoint3d&	operator/=(const GLFloatType value) { x/=value; y/=value; z/=value; return *this; };
		
		
		/*
		const WVPoint3d	operator+(const WVPoint2d &pt) { return WVPoint3d(x+pt.x,y+pt.y,z); };
		const WVPoint3d	operator-(const WVPoint2d &pt) { return WVPoint3d(x-pt.x,y-pt.y,z); };
		const WVPoint3d	operator*(const WVPoint2d &pt) { return WVPoint3d(x*pt.x,y*pt.y,z); };
		
		const WVPoint3d	operator+(const WVPoint3d &pt) { return WVPoint3d(x+pt.x,y+pt.y,z+pt.z); };
		const WVPoint3d	operator-(const WVPoint3d &pt) { return WVPoint3d(x-pt.x,y-pt.y,z-pt.z); };
		const WVPoint3d	operator*(const WVPoint3d &pt) { return WVPoint3d(x*pt.x,y*pt.y,z*pt.z); };
		
		const WVPoint3d	operator*(const GLFloatType value) { return WVPoint3d(x*value,y*value,z*value); };
		*/
};

class WVRect {
	public:
		WVRect(void) : origin(), size() {};
		WVRect(GLFloatType x, GLFloatType y, GLFloatType w, GLFloatType h) : origin(x, y), size(w, h) {};
		WVRect(const WVPoint2d &iorigin, const WVPoint2d &isize) : origin(iorigin), size(isize) {};
		
		GLFloatType		x()			{ return origin.x; };
		GLFloatType		y()			{ return origin.y; };
		GLFloatType		width()		{ return size.x; };
		GLFloatType		height()	{ return size.y; };

		WVPoint2d	origin,
					size;
		
		void	translate(GLFloatType x, GLFloatType y) { origin.translate(x, y); };
		void	translate(const WVPoint2d &pt) { origin.translate(pt); };
		void	translate(const WVPoint3d &pt) { origin.translate(pt.x, pt.y); };
		
		bool	contains(const WVPoint2d &pt) { return (pt.x >= origin.x && pt.x < (origin.x+size.x) && pt.y >= origin.y && pt.y < (origin.y+size.y)); };
		bool	contains(const WVPoint3d &pt) { return (pt.x >= origin.x && pt.x < (origin.x+size.x) && pt.y >= origin.y && pt.y < (origin.y+size.y)); };
		WVRect	intersect(const WVRect &r);
		bool	intersects(const WVRect &r);
		WVRect	rect_union(const WVRect &r);
		WVRect	floor(void) { return WVRect(::floor(origin.x), ::floor(origin.y), ::floor(size.x), ::floor(size.y)); };
		void	describe(void) { printf("WVRect = %.2f %.2f %.2f %.2f\n", origin.x, origin.y, size.x, size.y); };
		GLFloatType	area(void) { return size.x*size.y; };
		bool	empty(void) { return size.x*size.y <= 0.; };
		
		//	Subtracting a rect from another rect can generate at most four new rects
		int		subtract(WVRect &sub, WVRect *res /* pre-allocated array of 4 */);
		//static WVRect&	intersect(const WVRect &r1, const WVRect &r2);
};


class WVLine {
	public:
		WVLine(void) { pts[0] = WVPoint2d(); pts[1] = WVPoint2d(); };
		WVLine(WVPoint2d a, WVPoint2d b) { pts[0] = a; pts[1] = b; };
		WVLine(float ax, float ay, float bx, float by) { pts[0] = WVPoint2d(ax, ay); pts[1] = WVPoint2d(bx, by); };
		
		WVPoint2d	intersect(const WVLine &other, bool *valid=0);
		
		WVPoint2d	pts[2];
};


class WVCircle {
	public:
		WVCircle(void) : origin(), radius() {};
		WVCircle(GLFloatType x, GLFloatType y, GLFloatType iradius) : origin(x, y), radius(iradius) {};
		WVCircle(const WVPoint2d &iorigin, GLFloatType iradius) : origin(iorigin), radius(iradius) {};
		
		GLFloatType		x()			{ return origin.x; };
		GLFloatType		y()			{ return origin.y; };
		GLFloatType		r()			{ return radius; };

		WVPoint2d	origin;
		GLFloatType	radius;
		
		void	translate(GLFloatType x, GLFloatType y) { origin.translate(x, y); };
		void	translate(const WVPoint2d &pt) { origin.translate(pt); };
		void	translate(const WVPoint3d &pt) { origin.translate(pt.x, pt.y); };
		
		bool		contains(const WVPoint2d &pt) { return origin.distance(pt) <= radius; };
		bool		contains(const WVPoint3d &pt) { WVPoint2d tmp = WVPoint2d(pt.x, pt.y); return contains(tmp); };
		bool		contains(const WVCircle &c);
		bool		intersects(const WVCircle &c) { return origin.distance(c.origin) < (radius + c.radius); };
		WVCircle	circle_union(const WVCircle &c);
		WVRect		rect(void) { return WVRect(origin.x-radius, origin.y-radius, radius*2., radius*2.); };
		
		/*WVRect	intersect(const WVRect &r);
		bool	intersects(const WVRect &r);
		WVRect	rect_union(const WVRect &r);
		WVRect	floor(void) { return WVRect(::floor(origin.x), ::floor(origin.y), ::floor(size.x), ::floor(size.y)); };
		*/
		void		describe(void) { printf("Circle = %.2f %.2f Radius: %.2f\n", origin.x, origin.y, radius); };
		GLFloatType	area(void) { return M_PI*radius*radius; };
		bool		empty(void) { return radius <= 0.; };
};

#endif
