/*
 * Scene.h
 *
 *  Created on: Apr 30, 2011
 *      Author: arild
 */

#ifndef SCENE_H_
#define SCENE_H_

class Scene {
public:
	float x, y, w, h, scale;
	Scene(float x_, float y_, float w_, float h_, float virtualW, float virtualH);
	virtual ~Scene();
	void Load();
	void Unload();
};
#endif /* SCENE_H_ */
