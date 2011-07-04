/**
 * @file   BoidAxis.cpp
 * @Author Arild Nilsen
 * @date   May, 2011
 *
 * Entity that that draws the axes in the boid scene
 */

#include <GL/gl.h>
#include <sstream>
#include "Config.h"
#include "BoidAxis.h"
#include "Scene.h"

BoidAxis::BoidAxis()
{
}

BoidAxis::~BoidAxis()
{
}

void BoidAxis::Set(int start, int stop, int tickSize)
{
	_start = start;
	_stop = stop;
	_tickSize = tickSize;
}

void BoidAxis::Get(int *start, int *stop, int *tickSize)
{
}

void BoidAxis::OnInit()
{
	_font = new Font(4, true);
}

void BoidAxis::OnCleanup()
{
}

void BoidAxis::OnLoop()
{
}

void BoidAxis::OnRender()
{
	glColor3ub(0, 255, 0);

	float s = 0.3; // width and height of axises
	float x = -1 - s;
	float y = -1 - s;
	float w = 102 + s;
	float h = 102 + s;

	int numTicks = (_stop - _start) / _tickSize;
	float tw = 0.7;
	float th = 2.0;

	// Left
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x+s, y);
	glVertex2f(x+s, y+h+s);
	glVertex2f(x, y+h+s);
	glEnd();

	// Right
	glBegin(GL_QUADS);
	glVertex2f(x+w, y);
	glVertex2f(x+w+s, y);
	glVertex2f(x+w+s, y+h+s);
	glVertex2f(x+w, y+h+s);
	glEnd();

	// Bottom
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x+w+s, y);
	glVertex2f(x+w+s, y+s);
	glVertex2f(x, y+s);
	glEnd();

	for (int i = 0; i <= numTicks; i++) {
		float tx = (i * _tickSize) - tw/2;
		glBegin(GL_QUADS);
		glVertex2f(tx, y-th);
		glVertex2f(tx+tw, y-th);
		glVertex2f(tx+tw, y);
		glVertex2f(tx, y);
		glEnd();

		glColor3f(1, 0, 0);
		stringstream ss;
		ss << i * _tickSize;
		_font->RenderText(ss.str(), tx + tw/(float)2, -9);
		glColor3f(0, 1, 0);
	}

	_font->RenderText("Metric Utilization", 50, -15);

	glRotatef(90, 0, 0, 1);
	_font->RenderText("Metric Specific", -30, 50);

	// Top
	glBegin(GL_QUADS);
	glVertex2f(x, y+h);
	glVertex2f(x+w+s, y+h);
	glVertex2f(x+w+s, y+h+s);
	glVertex2f(x, y+h+s);
	glEnd();
}





