#ifndef IENTITY_H_
#define IENTITY_H_

using namespace std;

#include "TouchEvent.h"

class Entity {
public:
	float tx, ty;
	float width, height;
	bool supportsTouch;

	virtual ~Entity() {}
	virtual void OnLoop() = 0;
	virtual void OnRender() = 0;
	virtual void OnCleanup() = 0;

	virtual bool IsHit(float x, float y);
	virtual void HandleHit(TouchEvent &event);
};

#endif /* IENTITY_H_ */
