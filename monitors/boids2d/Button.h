/*
 * Button.h
 *
 *  Created on: May 20, 2011
 *      Author: arild
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include "Entity.h"

class Button : Entity {
public:
	Button(float x, float y, float w, float h);
	virtual ~Button();
	void SetCallback(void (*callback)());
	virtual void OnLoop();
	virtual void OnRender();
	virtual void OnCleanup();
	void HandleHit(TouchEvent &event);

private:
	bool _isActivated;
	void (*_callback)();
};

#endif /* BUTTON_H_ */
