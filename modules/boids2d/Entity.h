#ifndef IENTITY_H_
#define IENTITY_H_

using namespace std;

#include "touchmanager/STouchManager.h"
#include "TouchEvent.h"

enum Shape {
	QUAD,
	TRIANGLE,
	DIAMOND,
	POLYGON
};

class Entity {
public:
	static bool automaticallyAddToCurrentScene;
	float tx, ty;
	float width, height;
	bool supportsTouch;

	Entity();
	virtual ~Entity() {}
	virtual void OnInit() = 0;
	virtual void OnLoop() = 0;
	virtual void OnRender() = 0;
	virtual void OnCleanup() = 0;

	virtual bool IsHit(float x, float y);
	virtual void HandleHit(TouchEvent &event);
};

class EntityShape : virtual Entity {
public:
	Shape entityShape;
	bool centerShape;
	EntityShape();
	virtual ~EntityShape() {}
	void DrawEntityShape();
};

class EntityEvent : public virtual Entity {
public:
	EntityEvent();
	virtual ~EntityEvent() {}
	void HandleHit(TouchEvent &event);
	virtual void Up(float x, float y) = 0;
//	virtual void ScrollUp() = 0;
//	virtual void ScrollDown() = 0;
//	virtual void SwipeLeft() = 0;
//	virtual void SwipeRight() = 0;
};

#endif /* IENTITY_H_ */
