/*
 * Slider.h
 *
 *  Created on: May 24, 2011
 *      Author: arild
 */

#ifndef SLIDER_H_
#define SLIDER_H_

#include "Entity.h"

class Slider : Entity {
public:
	Slider();
	virtual ~Slider();

	// Required to implement. Entities inheriting from the slider
	// class should override these
	virtual void OnLoop();
	virtual void OnRender();
	virtual void OnCleanup();

	// Responsible generating slider events
	void HandleHit(TouchEvent &event);

	// Interface for entities inheriting, must be implemented
	virtual void SlideUpCallback() = 0;
	virtual void SlideDownCallback() = 0;
	virtual void SelectCallback(float x, float y) = 0;


};

#endif /* SLIDER_H_ */
