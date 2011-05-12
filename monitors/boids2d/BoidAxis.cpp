/*
 * Axis.cpp
 *
 *  Created on: May 11, 2011
 *      Author: arild
 */

#include <GL/gl.h>
#include "BoidAxis.h"

BoidAxis::BoidAxis()
{
	// TODO Auto-generated constructor stub

}

BoidAxis::~BoidAxis()
{
	// TODO Auto-generated destructor stub
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

void BoidAxis::OnCleanup()
{
}

void BoidAxis::OnLoop()
{
}

void BoidAxis::OnRender()
{
	glColor3ub(0, 255, 0);

	float w = 0.3;
	float l = -1 - w;
	float r = 100 + 1 + w;
	float b = -1 - w;
	float t = 100 + 1 + w;

	// Left
	glBegin(GL_QUADS);
	glVertex2f(l - w, b - w);
	glVertex2f(l, b - w);
	glVertex2f(l, t + w);
	glVertex2f(l - w, t + w);
	glEnd();

	// Right
	glBegin(GL_QUADS);
	glVertex2f(r, b - w);
	glVertex2f(r + w, b - w);
	glVertex2f(r + w, t + w);
	glVertex2f(r, t + w);
	glEnd();

	// Bottom
	glBegin(GL_QUADS);
	glVertex2f(l - w, b - w);
	glVertex2f(r + w, b - w);
	glVertex2f(r + w, b);
	glVertex2f(l - w, b);
	glEnd();
	//	_font->FaceSize(12, 200);
	//	_font->Render("Utilization");

	// Top
	glBegin(GL_QUADS);
	glVertex2f(l - w, t);
	glVertex2f(r + w, t);
	glVertex2f(r + w, t + w);
	glVertex2f(l - w, t + w);
	glEnd();
}





