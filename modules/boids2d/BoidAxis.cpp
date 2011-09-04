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
#include "BoidSharedContext.h"

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
	tx = 0;
	ty = 0;
	width = 100;
	height = 100;
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

	float s = 0.3; // width and height of axes
	float x = -1 - s;
	float y = -1 - s;
	float w = 102 + s;
	float h = 102 + s;

	// Left
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + s, y);
	glVertex2f(x + s, y + h + s);
	glVertex2f(x, y + h + s);
	glEnd();

	// Right
	glBegin(GL_QUADS);
	glVertex2f(x + w, y);
	glVertex2f(x + w + s, y);
	glVertex2f(x + w + s, y + h + s);
	glVertex2f(x + w, y + h + s);
	glEnd();

	// Bottom
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + w + s, y);
	glVertex2f(x + w + s, y + s);
	glVertex2f(x, y + s);
	glEnd();

	// Top
	glBegin(GL_QUADS);
	glVertex2f(x, y + h);
	glVertex2f(x + w + s, y + h);
	glVertex2f(x + w + s, y + h + s);
	glVertex2f(x, y + h + s);
	glEnd();

	// Ticks along x-axis and y-axis
	int numTicks = (_stop - _start) / _tickSize;
	w = 0.7;
	h = 2.0;
	float b = y;
	float l = x;
	for (int i = 0; i <= numTicks; i++) {

		// Render tick markers
		x = y = (i * _tickSize) - w / 2;
		glColor3f(0, 1, 0);
		glBegin(GL_QUADS);
		glVertex2f(x, b - h);
		glVertex2f(x + w, b - h);
		glVertex2f(x + w, b);
		glVertex2f(x, b);
		glEnd();

		glBegin(GL_QUADS);
		glVertex2f(l, y + w);
		glVertex2f(l - h, y + w);
		glVertex2f(l - h, y);
		glVertex2f(l, y);
		glEnd();

		// Render tick sizes
		glColor3f(1, 0, 0);
		stringstream ss;
		ss << i * _tickSize;
		_font->RenderText(ss.str(), x + w / (float) 2, -9);
		_font->RenderText(ss.str(), -10, y + w, false, true);
	}

	glColor3f(0, 1, 0);
	_font->RenderText("Metric Utilization", 50, -15);

	glPushMatrix();
	glRotatef(90.0f, 0, 0, 1.0f);
	_font->RenderText("Metric Specific", 50, 15, true, false);
	glPopMatrix();
}

void BoidAxis::Tap(float x, float y)
{
}

void BoidAxis::ScrollDown(float speed)
{
}

void BoidAxis::ScrollUp(float speed)
{
}

void BoidAxis::SwipeLeft(float speed)
{
	BoidSharedContext::tailLength -= speed;
	BoidSharedContext::tailLength = fmax(BoidSharedContext::tailLength, 0);
}

void BoidAxis::SwipeRight(float speed)
{
	BoidSharedContext::tailLength += speed;
	BoidSharedContext::tailLength = fmin(BoidSharedContext::tailLength, 100);
}

