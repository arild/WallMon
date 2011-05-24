/*
 * Button.h
 *
 *  Created on: May 20, 2011
 *      Author: arild
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include "IEntity.h"

class Button : IEntity {
public:
	Button(float x, float y, float w, float h);
	virtual ~Button();
	virtual void OnLoop();
	virtual void OnRender();
	virtual void OnCleanup();
	void HandleHit(TouchEvent &event);

private:
	bool _isActivated;
};

#endif /* BUTTON_H_ */
