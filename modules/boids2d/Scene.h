#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include <string>
#include "Entity.h"
#include <boost/thread/mutex.hpp>

using namespace std;

class Scene {
public:
	static vector<Scene *> scenes;
	static Scene *current;

	float x, y, w, h, scale;
	vector<Entity *> entityList;
	vector<Entity *> entityListInit;

	Scene(float x_, float y_, float w_, float h_, float virtualW, float virtualH);
	virtual ~Scene();
	void LoadVirtual();
	void LoadReal();
	void Unload();
	void RealToVirtualCoords(float realX, float realY, float *virtX, float *virtY);
	void Visualize();

	void AddEntity(Entity *entity);
	static void AddEntityToCurrent(Entity *entity);
	vector<Entity *> TestForEntityHits(float x, float y);
	void Run();

	static void AddScene(Scene *scene);
	static Scene *TestForSceneHit(float x, float y);
	static void RunAllScenes();

private:
	boost::mutex _entityMutex;
	static boost::mutex _sceneMutex;
};
#endif /* SCENE_H_ */
