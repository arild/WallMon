/*
 * Slider.cpp
 *
 *  Created on: May 24, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include "Slider.h"

Slider::Slider()
{
	// TODO Auto-generated constructor stub

}

Slider::~Slider()
{
	// TODO Auto-generated destructor stub
}

void Slider::OnInit()
{

}

void Slider::OnLoop()
{
}

void Slider::OnRender()
{
}

void Slider::OnCleanup()
{
}

void Slider::HandleHit(TouchEvent & event)
{
	if (event.isDown == false) {
		// Find item to be visually marked
		SelectCallback(event.x, event.y);
		return;
	}

	// Logic for sliding
	LOG(INFO) << "Radius: " << event.radius;
	LOG(INFO) << "Moved : " << event.movedDistance;

}





