#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include <string>
#include "Entity.h"
#include <boost/thread/mutex.hpp>

using namespace std;

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
	vector<Entity *> TestForEntityHits(float x, float y);
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
