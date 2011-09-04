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
vector<EntityEvent *> EntityEvent::_entities;

EntityEvent::EntityEvent()
{
	HandleTouchesPtr handle = &EntityEvent::_HandleTouchesCallback;
	touchManagerCallback_t callback = *(touchManagerCallback_t*) &handle;
	_touchManager = new STouchManager(this, callback);

	// Policy for detecting scrolls and swipes:
	// If the movement (delta) from previous touch exceeds 2 percent
	// of the height (scroll) or width (swipe)
	_maxScrollSpeed = -1;
	LOG(INFO) << "MAX: " << _maxScrollSpeed;
	_entities.push_back(this);
	_isFirstTime = true;
}

/**
 * Default implementation for entities touch event handling.
 *
 * The implementation forwards all events to the touch manager.
 */
void EntityEvent::HandleHit(TouchEvent & event)
{
	_touchManager->handleEvent(event.shoutEvent, event.x, event.y);
}

/**
 * Carries out touch event callbacks, and updates physics variables
 */
void EntityEvent::RunCallbacks()
{
	if (_scrollSpeed > _minScrollSpeed) {
	//LOG(INFO) << "SCROLL SPEED: " << _scrollSpeed;
		ScrollDown(_scrollSpeed);
		_scrollSpeed -= (_maxScrollSpeed * 0.05);
		if (_scrollSpeed < 0)
			_scrollSpeed = 0;
	}
	else if (_scrollSpeed < -_minScrollSpeed) {
		//LOG(INFO) << "SCROLL SPEED: " << _scrollSpeed;
		ScrollUp(-_scrollSpeed);
		_scrollSpeed += (_maxScrollSpeed * 0.05);
		if (_scrollSpeed > 0)
			_scrollSpeed = 0;
	}
	else
		_scrollSpeed = 0;
}

/**
 * Intended to be called by the main loop
 */
void EntityEvent::RunAllCallbacks()
{
	for (int i = 0; i < _entities.size(); i++)
		_entities[i]->RunCallbacks();
}

/**
 * Callback for the shout touch manager
 */
void EntityEvent::_HandleTouchesCallback(touchVector_t & down, touchVector_t & up)
{
	for (int i = 0; i < down.size(); i++) {
		TT_touch_state_t *obj = down[i];
		if (obj->wasUpdated) {
			obj->remove = false;
			_HandleTouch(*obj);
		}
	}
	for (int i = 0; i < up.size(); i++) {
		TT_touch_state_t *obj = up[i];
		if (obj->wasUpdated) {
			obj->remove = true;
			_HandleTouch(*obj);
		}
	}
}

/**
 * Implements policy for entity event callbacks
 */
void EntityEvent::_HandleTouch(TT_touch_state_t & event)
{
	if (_isFirstTime) {
		_isFirstTime = false;
		// Policies and thresholds not calculated
		_scrollThreshold = height * 0.03;
		_swipeThreshold = width * 0.03;
		_scrollSpeed = 0;
		_minScrollSpeed = height * 0.001;
		_maxScrollSpeed = height * 0.03;
	}

	if (event.remove) {
		if (event.movedDistance < 5) {
			LOG(INFO) << "TAP DETECTED";
			Tap(event.loc.x, event.loc.y);
		}
		return;
	}

	//LOG(INFO) << "EVENT: id=" << event.oid << " | " << "x=" << event.loc.x << " | y=" << event.loc.y << " | deltaX=" << event.delta.x << " | deltaY=" << event.delta.y << " | movedDistance=" << event.movedDistance << " remove=" << event.remove;

	if (fabs(event.delta.y) > _scrollThreshold) {
		// Scroll detected, update scroll speed, which is
		// read by entities every frame
		_scrollSpeed += event.delta.y;
		if (_scrollSpeed > _minScrollSpeed)
			_scrollSpeed = min(_scrollSpeed, _maxScrollSpeed);
		else if (_scrollSpeed < -_minScrollSpeed)
			_scrollSpeed = max(_scrollSpeed, -_maxScrollSpeed);
	}

}



