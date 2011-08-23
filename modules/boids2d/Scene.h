#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include <string>
#include "Entity.h"
#include <boost/thread/mutex.hpp>

using namespace std;

class Scene;
class EntityHit {
public:
        float virtX, virtY;
        Scene *scene;
        Entity *entity;

        EntityHit(float virtX_, float virtY_, Scene *scene_, Entity *entity_) :
                virtX(virtX_), virtY(virtY_), scene(scene_), entity(entity_) {}
};

class Scene {
public:
	static vector<Scene *> scenes;
	static Scene *current;
	vector<Entity *> entityList, entityListInit;

	Scene(float x, float y, float w, float h, float virtualW, float virtualH);
	virtual ~Scene();

	// Intended as public API
	float GetScale();
	static void AddScene(Scene *scene);
	void AddEntity(Entity *entity);
	static void AddEntityCurrent(Entity *entity);
	static vector<EntityHit> GetAllEntityHits(float x, float y);
	static void RunAllScenes();

	// API intended to be used by the Scene class itself
	// (Not made private due to the public static API)
	void LoadVirtual();
	void LoadReal();
	void Unload();
	void RealToVirtualCoords(float realX, float realY, float *virtX, float *virtY);
	void Visualize();
	bool IsSceneHit(float x, float y);
	vector<EntityHit> GetEntityHits(float x, float y);
	void Run();
private:
	float _x, _y, _w, _h, _scale;
	boost::mutex _entityMutex;
	static boost::mutex _sceneMutex;
};
#endif /* SCENE_H_ */
