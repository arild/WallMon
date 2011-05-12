/*
 * Scene.h
 *
 *  Created on: Apr 30, 2011
 *      Author: arild
 */

#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include "Entity.h"

using namespace std;

class Scene {
public:
	static Scene *current;
	float x, y, w, h, scale;
	vector<Entity *> entityList;

	Scene(float x_, float y_, float w_, float h_, float virtualW, float virtualH);
	virtual ~Scene();
	void Load();
	void Unload();
	void Run();
};
#endif /* SCENE_H_ */
