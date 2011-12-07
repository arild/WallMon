#ifndef IENTITY_H_
#define IENTITY_H_

#include <map>
#include <shout/event-types/touch-events.h>
#include "touchmanager/STouchManager.h"
#include "TouchEvent.h"

using namespace std;


enum Shape {
	QUAD,
	DIAMOND,
	TRIANGLE_UP,
	TRIANGLE_DOWN,
	TRIANGLE_LEFT,
	TRIANGLE_RIGHT,
	POLYGON
};

class Entity {
public:
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
	virtual void HandleHit(TT_touch_state_t &event);
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
	void HandleHit(TT_touch_state_t &event);
	void SetTapEventInterval(double seconds);
	void SetScrollEventInterval(double seconds);
	void SetSwipeEventInterval(double seconds);

	virtual void Tap(float x, float y);
//	virtual void TapHold(float x, float y) = 0;
	virtual void ScrollDown(float speed);
	virtual void ScrollUp(float speed);
	virtual void SwipeLeft(float speed);
	virtual void SwipeRight(float speed);
	virtual void Clap();
	static void RunAllCallbacks();
	void RunCallbacks();
private:
	void _RunScrollCallbacks();
	void _RunSwipeCallbacks();
	static vector<EntityEvent *> _entities;
	float _scrollThreshold, _swipeThreshold, _scrollSpeed, _swipeSpeed, _maxSpeed, _minSpeed;
	bool _isFirstTime;
	double _tapEventInterval, _previousTapEventTimestamp, _currentTapEventTimestamp;
	double _scrollEventInterval, _previousScrollEventTimestamp, _currentScrollEventTimestamp;
	double _swipeEventInterval, _previousSwipeEventTimestamp, _currentSwipeEventTimestamp;
	vector<TT_touch_state_t> _eventBuffer;
};

#endif /* IENTITY_H_ */
