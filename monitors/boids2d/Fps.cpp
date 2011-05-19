/**
 * @file   Network.cpp
 * @Author Arild Nilsen
 * @date   March, 2011
 *
 * Encapsulate timers for controlling the FPS of the main loop
 */


#include "Fps.h"
#include <iostream>
using namespace std;
Fps Fps::fpsControl;

Fps::Fps()
{
	_oldTime = 0;
	_lastTime = 0;
	_speedFactor = 0;
	_frames = 0;
	_numFrames = 0;
}

void Fps::OnLoop()
{
	unsigned int currentTime = SDL_GetTicks();
	if (currentTime > _oldTime + 1000) {
		// One second passed, reset state
		_oldTime = currentTime;
		_numFrames = _frames;
		_frames = 0;
	}
	unsigned int intendedTimeSpent = (unsigned int)(1000 / FPS);
	unsigned int actualTimeSpent = currentTime - _lastTime;
	if (intendedTimeSpent > actualTimeSpent) {
		// Only wait if we are ahead
		unsigned int delay = intendedTimeSpent - actualTimeSpent;
		SDL_Delay(delay);
		actualTimeSpent += delay;
		currentTime += delay;
	}

	// If we were ahead of time, this will calculate closely to 1.0
	_speedFactor = (actualTimeSpent / 1000.0f) * FPS;
	_lastTime = currentTime;
	_frames++;
}

int Fps::GetFPS()
{
	return _numFrames;
}

float Fps::GetSpeedFactor()
{
	return _speedFactor;
}
