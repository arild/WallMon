#ifndef ENTITY_H_
#define ENTITY_H_

#include <SDL/SDL.h>
#include <vector>
#include "Scene.h"

using namespace std;

class Entity {
public:
	static vector<Entity *> entityList;
	Scene *scene;
	float tx, ty;
	float speedx, speedy;
	float width, height;

	virtual ~Entity() {}
	virtual void OnLoop() = 0;
	virtual void OnRender() = 0;
	virtual void OnCleanup() = 0;
};

#endif /* ENTITY_H_ */
