#ifndef _FPS_H_
#define _FPS_H_

#include <SDL/SDL.h>

#define MAX_FPS 	35.f

class Fps {
public:
	static Fps fpsControl;
	Fps();
	void OnLoop();
	int GetFps();
	float GetSpeedFactor();
	unsigned int GetCurrentTimeInMsec();
	static float GetMaxIterationsPerTimeUnit(float msec);

private:
	unsigned int _oldTime;
	unsigned int _lastTime;
	unsigned int _currentTime;
	float _speedFactor;
	int _numFrames;
	int _frames;
};

#endif
