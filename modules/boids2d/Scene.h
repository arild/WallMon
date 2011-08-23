#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include <string>
#include <boost/thread/mutex.hpp>
#include "Entity.h"

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
	static Scene *current;

	Scene(float x, float y, float w, float h, float virtualW, float virtualH);
	virtual ~Scene();
	Scene *CreateSubScene(float x, float y, float w, float h, float virtualW, float virtualH);
	void LoadVirtual();
	void LoadReal();
	void Unload();
	float GetScale();
	void RealToVirtualCoords(float realX, float realY, float *virtX, float *virtY);
	void VirtualToRealCoords(float virtX, float virtY, float *realX, float *realY);
	void Visualize();
	void AddEntity(Entity *entity);
	static void AddEntityCurrent(Entity *entity);
	vector<EntityHit> TestForEntityHits(float x, float y);
	void Run();
private:
	float _x, _y, _w, _h, _virtualW, _virtualH, _scale;
	vector<Entity *> _entityList, _entityListInit;
	vector<Scene *> _subScenes;

	void _RunEntities();
	void _RunSubScenes();
	bool _IsSceneHit(float x, float y);
	boost::mutex _entityMutex, _sceneMutex;
};
#endif /* SCENE_H_ */
