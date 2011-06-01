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

class ControlPanel : Entity {
public:
	ControlPanel();
	virtual ~ControlPanel();
	virtual void OnLoop();
	virtual void OnRender();
	virtual void OnCleanup();
	static void CpuBoidButtonCallback();
	static void MemoryBoidButtonCallback();
	static void NetworkBoidButtonCallback();
	static void StorageBoidButtonCallback();

	static void BoidTailCallback();

private:
	Font *_font;
};

#endif /* BOIDDESCRIPTION_H_ */
