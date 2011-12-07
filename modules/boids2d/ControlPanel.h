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


class ControlPanel: public EntityEvent {
public:
	virtual void OnInit();
	virtual void OnLoop();
	virtual void OnRender();
	virtual void OnCleanup();
	void Tap(float x, float y);
	void Clap();

	static void CpuBoidButtonCallback();
	static void MemoryBoidButtonCallback();
	static void NetworkBoidButtonCallback();
	static void StorageBoidButtonCallback();
private:
	Font *_font;
	GLuint _textureId;
	bool _showLegend, _imageIsLoaded;
};

#endif /* BOIDDESCRIPTION_H_ */
