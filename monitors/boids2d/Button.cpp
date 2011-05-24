/*
 * Button.cpp
 *
 *  Created on: May 20, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include "Button.h"
#include <GL/gl.h>

Button::Button(float x, float y, float w, float h)// : tx(x), ty(y), width(w), height(h)
{
	tx = x;
	ty = y;
	width = w;
	height = h;
	_isActivated = false;
}

Button::~Button()
{
}

void Button::OnLoop()
{
}

void Button::OnRender()
{
	if (_isActivated)
		glColor3f(0, 1, 0);
	else
		glColor3f(1, 1, 1);

	glBegin(GL_QUADS);
	glVertex2f(tx, ty);
	glVertex2f(tx + width, ty);
	glVertex2f(tx + width, ty + height);
	glVertex2f(tx, ty + height);
	glEnd();
}

void Button::OnCleanup()
{
}

void Button::HandleHit(TouchEvent &event)
{
	if (event.isDown == true)
		return;

	if (_isActivated)
		_isActivated = false;
	else
		_isActivated = true;
}

