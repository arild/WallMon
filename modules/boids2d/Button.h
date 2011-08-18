/*
 * Button.h
 *
 *  Created on: May 20, 2011
 *      Author: arild
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include "Entity.h"

class Button : virtual Entity, EntityShape {
public:
	Button(float x, float y, float w, float h, Shape buttonShape=QUAD, bool slowActivation=true);
	virtual ~Button();
	void SetCallback(void (*callback)());
	void ButtonClick();
	virtual void OnInit();
	virtual void OnLoop();
	virtual void OnRender();
	virtual void OnCleanup();
	void HandleHit(TouchEvent &event);

private:
	bool _slowActivation;
	double _timestampSec;
	bool _animationOn;
	float _offset;
	float _stepSize;
	bool _isActivated;
	void (*_callback)();
};

#endif /* BUTTON_H_ */
