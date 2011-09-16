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

class ControlPanel: public Entity {
public:
	virtual void OnInit();
	virtual void OnLoop();
	virtual void OnRender();
	virtual void OnCleanup();

	static void CpuBoidButtonCallback();
	static void MemoryBoidButtonCallback();
	static void NetworkBoidButtonCallback();
	static void StorageBoidButtonCallback();

	static void BoidTailCallback();
private:
	Font *_font, *_fontLarge;
};

#endif /* BOIDDESCRIPTION_H_ */
