/*
 * ControlPanel.h
 *
 *  Created on: May 13, 2011
 *      Author: arild
 */

#ifndef CONTROLPANEL_H_
#define CONTROLPANEL_H_

#include "IEntity.h"

enum Buttons {
   Tail,
   Pause
};

class ControlPanel : IEntity {
public:
	ControlPanel();
	virtual ~ControlPanel();

	void TailButtonClick();
	bool IsTailButton();

	virtual void OnLoop();
	virtual void OnRender();
	virtual void OnCleanup();
};

#endif /* CONTROLPANEL_H_ */
