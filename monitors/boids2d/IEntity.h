#ifndef IENTITY_H_
#define IENTITY_H_

#include <SDL/SDL.h>
#include <vector>

using namespace std;

class IEntity {
public:
	float tx, ty;
	float width, height;

	virtual ~IEntity() {}
	virtual void OnLoop() = 0;
	virtual void OnRender() = 0;
	virtual void OnCleanup() = 0;
};

#endif /* IENTITY_H_ */
