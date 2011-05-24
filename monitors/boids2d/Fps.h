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

private:
	int _oldTime;
	int _lastTime;
	float _speedFactor;
	int _numFrames;
	int _frames;
};

#endif
