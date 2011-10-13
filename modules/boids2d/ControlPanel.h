/*
 * BoidDescription.h
 *
 *  Created on: May 29, 2011
 *      Author: arild
 */

#ifndef BOIDDESCRIPTION_H_
#define BOIDDESCRIPTION_H_

#include <GL/gl.h>
#include "Font.h"
#include "Entity.h"
#include <string.h>

using namespace std;

class ControlPanel: public Entity {
public:
	virtual void OnInit();
	virtual void OnLoop();
	virtual void OnRender();
	virtual void OnCleanup();
	virtual void Tap(float x, float y);
	virtual void ScrollDown(float speed);
	virtual void ScrollUp(float speed);
	virtual void SwipeLeft(float speed);
	virtual void SwipeRight(float speed);

	static void CpuBoidButtonCallback();
	static void MemoryBoidButtonCallback();
	static void NetworkBoidButtonCallback();
	static void StorageBoidButtonCallback();
private:
	Font *_font;
};

#endif /* BOIDDESCRIPTION_H_ */
