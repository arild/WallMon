#ifndef ENTITY_H_
#define ENTITY_H_

#include <SDL/SDL.h>
#include <vector>

class Entity {
public:
	static std::vector<Entity *> entityList;

	float tx, ty;
	float speedx, speedy;
	float width, height;

	virtual ~Entity() {}
	virtual void OnLoop() = 0;
	virtual void OnRender(SDL_Surface *screen) = 0;
	virtual void OnCleanup() = 0;

protected:
	SDL_Surface* screen;
};

#endif /* ENTITY_H_ */
