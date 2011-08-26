#include <GL/gl.h>
#include <glog/logging.h>
#include "Entity.h"
#include "Scene.h"

bool Entity::automaticallyAddToCurrentScene = false;

Entity::Entity()
{
	tx, ty, width, height = 0;
	if (automaticallyAddToCurrentScene)
		Scene::AddEntityCurrent(this);
}

bool Entity::IsHit(float x, float y)
{
	if (x >= tx && x <= tx + width && y >= ty && y <= ty + height)
		return true;
	return false;
}

void Entity::HandleHit(TouchEvent &event)
{

}

EntityShape::EntityShape()
{
	centerShape = false;
}

void EntityShape::DrawEntityShape()
{
	float x, y;
	if (centerShape) {
		x = tx - (width / 2);
		y = ty - (height / 2);
	} else {
		x = tx;
		y = ty;
	}
	switch (entityShape)
	{
	case QUAD:
		glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x + width, y);
		glVertex2f(x + width, y + height);
		glVertex2f(x, y + height);
		glEnd();
		break;
	case TRIANGLE:
		glBegin(GL_TRIANGLES);
		glVertex2f(x, y);
		glVertex2f(x + width, y);
		glVertex2f(x + width / 2, y + height);
		glEnd();
		break;
	case DIAMOND:
		glBegin(GL_QUADS);
		glVertex2f(x, y + height / 2);
		glVertex2f(x + width / 2, y);
		glVertex2f(x + width, y + height / 2);
		glVertex2f(x + width / 2, y + height);
		glEnd();
		break;
	case POLYGON:
		glBegin(GL_POLYGON);
		glVertex2f(x, y + height / 4);
		glVertex2f(x + width / 4, y);
		glVertex2f(x + width - width / 4, y);
		glVertex2f(x + width, y + height / 4);
		glVertex2f(x + width, y + height - height / 4);
		glVertex2f(x + width - width / 4, y + height);
		glVertex2f(x + width / 4, y + height);
		glVertex2f(x, y + height - height / 4);
		glEnd();
		break;
	}
}

typedef void (EntityEvent::*HandleTouchesPtr)(touchVector_t &down, touchVector_t &up);

EntityEvent::EntityEvent()
{
	HandleTouchesPtr handle = &EntityEvent::HandleTouchesCallback;
	touchManagerCallback_t callback = *(touchManagerCallback_t*) &handle;
	_touchManager = new STouchManager(this, callback);

	// Policy for detecting scrolls and swipes:
	// If the movement (delta) from previous touch exceeds 2 percent
	// of the height (scroll) or width (swipe)
	_scrollThreshold = height * 0.03;
	_swipeThreshold = width * 0.03;
}

/**
 * Default implementation that detects up, scroll up and down, and
 * swipe left and right events, and calls associated handlers.
 */
void EntityEvent::HandleHit(TouchEvent & event)
{
	if (event.shoutEvent == NULL) {
		TT_touch_state_t _event;
		_event.loc = WVPoint2d(event.x, event.y);
		_event.remove = event.isUp;
		HandleTouch(_event);
	}
	else
		_touchManager->handleEvent(event.shoutEvent, event.x, event.y);
}

void EntityEvent::HandleTouchesCallback(touchVector_t & down, touchVector_t & up)
{
	for (int i = 0; i < down.size(); i++) {
		TT_touch_state_t *obj = down[i];
		if (obj->wasUpdated) {
			obj->remove = false;
			HandleTouch(*obj);
		}
	}
	for (int i = 0; i < up.size(); i++) {
		TT_touch_state_t *obj = up[i];
		if (obj->wasUpdated) {
			obj->remove = true;
			HandleTouch(*obj);
		}
	}
}

void EntityEvent::HandleTouch(TT_touch_state_t & event)
{
	LOG(INFO) << "EVENT: id=" << event.oid << " | " << "x=" << event.loc.x << " | y=" << event.loc.y << " | deltaX=" << event.delta.x << " | deltaY=" << event.delta.y << " | movedDistance=" << event.movedDistance << " remove=" << event.remove;
//	LOG(INFO) << "Time since last event: " event


	if (event.remove) {
		LOG(INFO) << "TAP DETECTED";
		Tap(event.loc.x, event.loc.y);
		return;
	}
	if (event.delta.y > _scrollThreshold) {
		// User is moving hand upwards, which results
		// in a scroll down
		LOG(INFO) << "SCROLL DOWN DETECTED";
		ScrollDown(event.delta.y);
	}
	else if (event.delta.y < -_scrollThreshold) {
		LOG(INFO) << "SCROLL UP DETECTED";
		ScrollUp(event.delta.y);
	}

}





