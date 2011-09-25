/**
 * @file   BoidAxis.cpp
 * @Author Arild Nilsen
 * @date   May, 2011
 *
 * Entity that that draws the axes in the boid scene
 */

#include <glog/logging.h>
#include <GL/gl.h>
#include <sstream>
#include "math.h"
#include "System.h"
#include "Config.h"
#include "BoidAxis.h"
#include "Scene.h"
#include "BoidSharedContext.h"


using namespace std;

const float S = 0.3; // width and height of axes
const float X = -1 - S;
const float Y = -1 - S;
const float W = 102 + S;
const float H = 102 + S;


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

	SetScrollEventInterval(1);
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

	// Left
	glBegin(GL_QUADS);
	glVertex2f(X, Y);
	glVertex2f(X + S, Y);
	glVertex2f(X + S, Y + H + S);
	glVertex2f(X, Y + H + S);
	glEnd();

	// Right
	glBegin(GL_QUADS);
	glVertex2f(X + W, Y);
	glVertex2f(X + W + S, Y);
	glVertex2f(X + W + S, Y + H + S);
	glVertex2f(X + W, Y + H + S);
	glEnd();

	// Bottom
	glBegin(GL_QUADS);
	glVertex2f(X, Y);
	glVertex2f(X + W + S, Y);
	glVertex2f(X + W + S, Y + S);
	glVertex2f(X, Y + S);
	glEnd();

	// Top
	glBegin(GL_QUADS);
	glVertex2f(X, Y + H);
	glVertex2f(X + W + S, Y + H);
	glVertex2f(X + W + S, Y + H + S);
	glVertex2f(X, Y + H + S);
	glEnd();

	// Ticks along bottom x-axis and left y-axis
	int numTicks = (_stop - _start) / _tickSize;
	float w = 0.7;
	float h = 1.4;
	float b = Y;
	float l = X;
	for (int i = 0; i <= numTicks; i++) {

		// Render tick markers
		float x = (i * _tickSize) - w / 2;
		float y = x;
		glColor3f(0, 1, 0);
		if (!BoidSharedContext::useLogarithmicAxis) {
			glBegin(GL_QUADS);
			glVertex2f(x, b - h);
			glVertex2f(x + w, b - h);
			glVertex2f(x + w, b);
			glVertex2f(x, b);
			glEnd();
		}

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
		if (!BoidSharedContext::useLogarithmicAxis)
			_font->RenderText(ss.str(), x + w / (float) 2, -9);
		_font->RenderText(ss.str(), -10, y + w, false, true);
	}

	// Ticks along top x-axis and right y-axis
	float top = Y + H;
	float right = X + W;
	for (int i = 0; i < 9; i++) {
		// Render tick markers
		float step = i * (float)(100 / (float)8) - w/2;
		glColor3f(0, 1, 0);

		if (BoidSharedContext::useLogarithmicAxis) {
			glColor3f(0, 1, 0);
			glBegin(GL_QUADS);
			glVertex2f(step, b - h);
			glVertex2f(step + w, b - h);
			glVertex2f(step + w, b);
			glVertex2f(step, b);
			glEnd();
		}

		// Skip the ticks and numbers along right vertical axis
//		glBegin(GL_QUADS);
//		glVertex2f(right, step);
//		glVertex2f(right + h, step);
//		glVertex2f(right + h, step + w);
//		glVertex2f(right, step + w);
//		glEnd();
//		// Render tick sizes
		glColor3f(1, 0, 0);
//		_font->RenderText(ss.str(), 105, step + w / (float) 2, false, true);
		stringstream ss;
		if (i == 0)
			ss << 0;
		else
			ss << (float)pow((float)2, i - 1);
		if (BoidSharedContext::useLogarithmicAxis)
			_font->RenderText(ss.str(), step + w / (float) 2, -9);
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
	BoidSharedContext::useLogarithmicAxis = !BoidSharedContext::useLogarithmicAxis;
}

void BoidAxis::ScrollUp(float speed)
{
	BoidSharedContext::useLogarithmicAxis = !BoidSharedContext::useLogarithmicAxis;
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

void BoidAxis::_DrawLinearTicks()
{

}

void BoidAxis::_DrawLogarithmicTicks()
{
	for (int i = 0; i < 10; i++) {

		// Render tick markers
//		x = y = (i * _tickSize) - w / 2;
//		glColor3f(0, 1, 0);
//		glBegin(GL_QUADS);
//		glVertex2f(x, b - h);
//		glVertex2f(x + w, b - h);
//		glVertex2f(x + w, b);
//		glVertex2f(x, b);
//		glEnd();
//
//		glBegin(GL_QUADS);
//		glVertex2f(l, y + w);
//		glVertex2f(l - h, y + w);
//		glVertex2f(l - h, y);
//		glVertex2f(l, y);
//		glEnd();
//
//		// Render tick sizes
//		glColor3f(1, 0, 0);
//		stringstream ss;
//		ss << i * _tickSize;
//		_font->RenderText(ss.str(), x + w / (float) 2, -9);
//		_font->RenderText(ss.str(), -10, y + w, false, true);
	}
}
