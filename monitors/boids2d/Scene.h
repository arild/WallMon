#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include <string>
#include "IEntity.h"

using namespace std;

class Scene {
public:
	static vector<Scene *> scenes;
	static Scene *current;

	float x, y, w, h, scale;
	float scaleX, scaleY;
	vector<IEntity *> entityList;

	Scene(float x_, float y_, float w_, float h_, float virtualW, float virtualH);
	virtual ~Scene();
	void LoadVirtual();
	void LoadReal();
	void Unload();
	void RealToVirtualCoords(float realX, float realY, float *virtX, float *virtY);
	void Visualize();

	// Assumes that coordinates are within scene of the intantiated object
	vector<IEntity *> TestForEntityHits(float x, float y);
	static Scene *TestForSceneHit(float x, float y);

	void Run();
};
#endif /* SCENE_H_ */
