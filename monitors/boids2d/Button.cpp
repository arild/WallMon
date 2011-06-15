/*
 * Button.cpp
 *
 *  Created on: May 20, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include "Button.h"
#include <GL/gl.h>
#include "System.h"
#include "Fps.h"

bool activateButton = false;

Button::Button(float x, float y, float w, float h, Shape buttonShape, bool slowActivation)
{
	tx = x;
	ty = y;
	width = w;
	height = h;
	entityShape = buttonShape;
	centerShape = false;
	_isActivated = false;
	_offset = 0;
	_timestampSec = System::GetTimeInSec();
	_slowActivation = slowActivation;

	// Try to achieve 500 msec activation
	float iterationsPer500Msec = Fps::GetMaxIterationsPerTimeUnit(500);
	_stepSize = (float)width / iterationsPer500Msec;
	_animationOn = false;
}

Button::~Button()
{
}

void Button::SetCallback(void(*callback)())
{
	_callback = callback;
}

void Button::ButtonClick()
{
	if (_isActivated) {
		_isActivated = false;
		_offset = 0;
	}
	else {
		_isActivated = true;
		_offset = width;
	}
	_callback();
}

void Button::OnLoop()
{
	if (_slowActivation == false)
		return;

	if (_animationOn == false)
		return;

	if (System::GetTimeInSec() - _timestampSec < 0.2) {
		// New event within threshold - Move forward with animation
		if (_isActivated)
			_offset -= _stepSize;
		else
			_offset += _stepSize;
	}
	else {
		// No event within threshold - Move backward with animation
		if (_isActivated)
			_offset += _stepSize;
		else
			_offset -= _stepSize;
	}

	// Check if activation / de-activation limits have been reached
	if (_offset <= 0) {
		_offset = 0;
		_animationOn = false;
		ButtonClick();
	}
	if (_offset >= width) {
		_offset = width;
		_animationOn = false;
		ButtonClick();
	}
}

void Button::OnRender()
{
	if (_slowActivation) {
		float widthSave = width;
		float txSave = tx;

		glColor3f(0, 1, 0);
		width = _offset;
		DrawEntityShape();

		glColor3f(1, 1, 1);
		tx += _offset;
		width = widthSave - _offset;
		DrawEntityShape();

		width = widthSave;
		tx = txSave;
	} else {
		if (_isActivated)
			glColor3f(0, 1, 0);
		else
			glColor3f(1, 1, 1);
		DrawEntityShape();
//		glBegin(GL_QUADS);
//		glVertex2f(tx, ty);
//		glVertex2f(tx + width, ty);
//		glVertex2f(tx + width, ty + height);
//		glVertex2f(tx, ty + height);
//		glEnd();
	}
}

void Button::OnCleanup()
{
}

void Button::HandleHit(TouchEvent &event)
{
	if (_slowActivation) {
		if (_animationOn == false && System::GetTimeInSec() - _timestampSec < 1.0)
			// Wait time after a button has been clicked
			return;
		_animationOn = true;
		_timestampSec = System::GetTimeInSec();//(double)event.timestampSec;
	} else {
		if (event.isDown == true)
			return;
		ButtonClick();
	}
}




