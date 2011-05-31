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

class BoidDescription : Entity {
public:
	BoidDescription();
	virtual ~BoidDescription();
	virtual void OnLoop();
	virtual void OnRender();
	virtual void OnCleanup();
	static void CpuBoidButtonCallback();
	static void MemoryBoidButtonCallback();
private:
	Font *_font;
};

#endif /* BOIDDESCRIPTION_H_ */
