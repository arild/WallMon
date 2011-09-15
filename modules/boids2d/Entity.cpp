#include <GL/gl.h>
#include <glog/logging.h>
#include "Entity.h"
#include "Scene.h"

Entity::Entity()
{
	tx, ty, width, height = 0;
}

bool Entity::IsHit(float x, float y)
{
	if (x >= tx && x <= tx + width && y >= ty && y <= ty + height)
		return true;
	return false;
}

void Entity::HandleHit(TT_touch_state_t &event)
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
		x = tx - (width / (float)2);
		y = ty - (height / (float)2);
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

vector<EntityEvent *> EntityEvent::_entities;

EntityEvent::EntityEvent()
{
	_entities.push_back(this);
	_isFirstTime = true;
	_scrollSpeed = 0;
	_swipeSpeed = 0;

	_scrollEventInterval = -1;
	_swipeEventInterval = -1;
	_previousScrollEventTimestamp = -1;
	_previousSwipeEventTimestamp = -1;
}

/**
 * Default implementation for entities touch event handling.
 *
 * Implements policy for entity event callbacks
 */
void EntityEvent::HandleHit(TT_touch_state_t & event)
{

//	LOG(INFO) << "Received event id " << event.oid;// << " | " << "x=" << event.loc.x << " | y=" << event.loc.y << " | deltaX=" << event.delta.x << " | deltaY=" << event.delta.y << " | movedDistance=" << event.movedDistance << " remove=" << event.remove;
	if (_isFirstTime) {
		_isFirstTime = false;
		// Policies and thresholds not calculated
		_scrollThreshold = height * 0.03;
		_swipeThreshold = width * 0.03;
		_minSpeed = height * 0.001;
		_maxSpeed = height * 0.03;
	}

	if (event.remove) {
		if (event.movedDistance < 5) {
			//			LOG(INFO) << "TAP DETECTED";
			Tap(event.loc.x, event.loc.y);
		}
		return;
	}

	float fy = fabs(event.delta.y);
	float fx = fabs(event.delta.x);
	if (fy < _scrollThreshold && fx < _swipeThreshold)
		return;

	// TODO: Take varied x and y axis into account
	bool isScroll = false;
	if (fy > fx)
		isScroll = true;

	// Scroll detected, update scroll speed, which is
	// read by entities every frame
	if (isScroll) {
		_currentScrollEventTimestamp = event.lastUpdated;
		_scrollSpeed += event.delta.y;
		if (_scrollSpeed > _minSpeed)
			_scrollSpeed = min(_scrollSpeed, _maxSpeed);
		else if (_scrollSpeed < -_minSpeed)
			_scrollSpeed = max(_scrollSpeed, -_maxSpeed);
	} else {
		_currentSwipeEventTimestamp = event.lastUpdated;
		_swipeSpeed += event.delta.x;
		if (_swipeSpeed > _minSpeed)
			_swipeSpeed = min(_swipeSpeed, _maxSpeed);
		else if (_swipeSpeed < -_minSpeed)
			_swipeSpeed = max(_swipeSpeed, -_maxSpeed);
	}
}

void EntityEvent::SetScrollEventInterval(double seconds)
{
	_scrollEventInterval = seconds;
}

void EntityEvent::SetSwipeEventInterval(double seconds)
{
	_swipeEventInterval = seconds;
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
 * Carries out touch event callbacks, and updates physics variables
 */
void EntityEvent::RunCallbacks()
{
	if (_currentScrollEventTimestamp - _previousScrollEventTimestamp > _scrollEventInterval) {
		_previousScrollEventTimestamp = _currentScrollEventTimestamp;
		_RunScrollCallbacks();
	}
	if (_currentSwipeEventTimestamp - _previousSwipeEventTimestamp > _swipeEventInterval) {
		_previousSwipeEventTimestamp = _currentSwipeEventTimestamp;
		_RunSwipeCallbacks();
	}
}


void EntityEvent::_RunScrollCallbacks()
{
	if (_scrollSpeed > _minSpeed) {
		ScrollDown(_scrollSpeed);
		_scrollSpeed -= (_maxSpeed * 0.05);
		if (_scrollSpeed < 0)
			_scrollSpeed = 0;
	} else if (_scrollSpeed < -_minSpeed) {
		ScrollUp(-_scrollSpeed);
		_scrollSpeed += (_maxSpeed * 0.05);
		if (_scrollSpeed > 0)
			_scrollSpeed = 0;
	} else
		_scrollSpeed = 0;
}

void EntityEvent::_RunSwipeCallbacks()
{
	if (_swipeSpeed > _minSpeed) {
		SwipeRight(_swipeSpeed);
		_swipeSpeed -= (_maxSpeed * 0.05);
		if (_swipeSpeed < 0)
			_swipeSpeed = 0;
	} else if (_swipeSpeed < -_minSpeed) {
		SwipeLeft(-_swipeSpeed);
		_swipeSpeed += (_maxSpeed * 0.05);
		if (_swipeSpeed > 0)
			_swipeSpeed = 0;
	} else
		_swipeSpeed = 0;
}

