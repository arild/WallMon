/**
 * Implements a scene abstraction that allows entities to work and
 * operate on their own coordinate system.
 */

#include <GL/gl.h>
#include <boost/foreach.hpp>
#include <algorithm>
#include "Scene.h"
#include <glog/logging.h>

typedef boost::mutex::scoped_lock scoped_lock;

vector<Scene *> Scene::scenes;
Scene *Scene::current = NULL;
boost::mutex Scene::_sceneMutex;


/**
 * Calculates the common scale factor in BOTH x and y direction based on virtual
 * and real coordinate systems. The common scale factor is the smallest scale
 * factor of the two.
 *
 * This results in an abstraction with several consequences. In the example below,
 * we have a coordinate system with resolution 1000x500, hence the aspect ratio 2:1.
 * If we make a scene that covers the whole coordinate system, with a local virtual
 * coordinate system with resolution 100x100:
 *
 * 		Scene(0, 0, 1000, 500, 100, 100)
 *
 * And then create and renders a square starting in bottom-left (0,0) and that covers
 * the whole virtual coordinate system (width = 100 and height = 100), this would be the result:
 *       _______________			  _______________
 *		|				|      	     |///////|		|
 *  500 |				|  ->  '100' |///////|		|
 * 		|				|		     |///////|		|
 * 		 ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾              ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 * 			   1000						   '100'
 *
 * As we can see, physically, only half of the screen will be filled... Does this make sense?
 * It does, if one draws a square, the output should be a square, and not a rectangle (as would be
 * the case if the whole screen had been filled).
 *
 */
Scene::Scene(float x, float y, float w, float h, float virtualW_, float virtualH_) : _x(x), _y(y), _w(w), _h(h)
{
	float scaleX = w / (float)virtualW_;
	float scaleY = h / (float) virtualH_;
	_scale = std::min(scaleX, scaleY);
}

Scene::~Scene()
{
}

float Scene::GetScale()
{
	return _scale;
}

void Scene::AddScene(Scene *scene)
{
	scoped_lock(_sceneMutex);
	scenes.push_back(scene);
}

void Scene::AddEntity(Entity *entity)
{
	scoped_lock(_entityMutex);
	entityListInit.push_back(entity);
}

void Scene::AddEntityCurrent(Entity *entity)
{
	if (current == NULL)
		return;
	current->_entityMutex.lock();
	current->entityListInit.push_back(entity);
	current->_entityMutex.unlock();
}

void Scene::RemoveEntity(Entity *entity)
{
	scoped_lock(_entityMutex);
	for (int i = 0; i < entityList.size(); i++)
		if (entityList[i] == entity)
			entityList.erase(entityList.begin() + i);
}

vector<EntityHit> Scene::GetAllEntityHits(float x, float y)
{
	scoped_lock(_sceneMutex);
    vector<EntityHit> resultAllScenes;
    for (int i = 0; i < Scene::scenes.size(); i++) {
    	Scene *s = Scene::scenes[i];
    	if (s->IsSceneHit(x, y) == false)
        	continue;
		// Test for entity hits within a single scene.
		// In this scenario, we do not need to load the virtual scene, only provide
		// entities with coordinates in their virtual coordinate system
		float virtX, virtY;
		s->RealToVirtualCoords(x, y, &virtX, &virtY);
        vector<EntityHit> resultScene = s->GetEntityHits(virtX, virtY);

        // Combine results
        for (int j = 0; j < resultScene.size(); j++)
        	resultAllScenes.push_back(resultScene[j]);
    }
    return resultAllScenes;
}

void Scene::RunAllScenes()
{
	scoped_lock(_sceneMutex);
	for (int i = 0; i < Scene::scenes.size(); i++)
		Scene::scenes[i]->Run();
}


/**
 * Loads the local coordinate system to the scene.
 * (0,0) is bottom-left of scene.
 */
void Scene::LoadVirtual()
{
	glPushMatrix();
	glTranslatef(_x, _y, 0);
	glScalef(_scale, _scale, 1);
}

/**
 * Loads the global coordinate system, however, (0,0) is still
 * bottom-left of current scene.
 */
void Scene::LoadReal()
{
	glPushMatrix();
	glTranslatef(_x, _y, 0);
	glScalef(1., 1., 1.);
}

void Scene::Unload()
{
	glPopMatrix();
}

void Scene::RealToVirtualCoords(float realX, float realY, float *virtX, float *virtY)
{
	float localX = realX - _x;
	float localY = realY - _y;
	*virtX = localX * (1 / (float)_scale);
	*virtY = localY * (1 / (float)_scale);
}


bool Scene::IsSceneHit(float x, float y)
{
        if (x >= _x && x <= _x + _w && y >= _y && y <= _y + _h)
                return true;
        return false;
}

/**
 * Takes coordinates according to scene coordinate system and returns all
 * entities within the scene that overlaps with given coordinates
 */
vector<EntityHit> Scene::GetEntityHits(float x, float y)
{
	vector<EntityHit> result;
	_entityMutex.lock();
	for (int i = 0; i < entityList.size(); i++)
			if (entityList[i]->IsHit(x, y))
				result.push_back(EntityHit(x, y, this, entityList[i]));
	_entityMutex.unlock();
	return result;
}

void Scene::Run()
{
	scoped_lock(_entityMutex);
	current = this;
	LoadVirtual();
	while (entityListInit.size() > 0) {
		Entity *e = entityListInit.back();
		entityListInit.pop_back();
		entityList.push_back(e);
		e->OnInit();
	}
	for (int i = 0; i < entityList.size(); i++)
		entityList[i]->OnLoop();
	for (int i = 0; i < entityList.size(); i++)
		entityList[i]->OnRender();
	Unload();
}

int Scene::GetTotalNumEntities()
{
	scoped_lock(_sceneMutex);
	scoped_lock(_entityMutex);
	int numEntities = 0;
	for (int i = 0; i < scenes.size(); i++)
		numEntities += scenes[i]->entityList.size();
	return numEntities;
}

/**
 * Draws a blue bounding box around the scene
 */
void Scene::Visualize()
{
	LoadReal();
	float s = 5;

	glColor3f(0, 0, 1);

	// Left
	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(s, 0);
	glVertex2f(s, _h);
	glVertex2f(0, _h);
	glEnd();

	// Right
	glBegin(GL_QUADS);
	glVertex2f(_w-s, 0);
	glVertex2f(_w, 0);
	glVertex2f(_w, _h);
	glVertex2f(_w-s, _h);
	glEnd();

	// Bottom
	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(_w, 0);
	glVertex2f(_w, s);
	glVertex2f(0, s);
	glEnd();

	// Top
	glBegin(GL_QUADS);
	glVertex2f(0, _h-s);
	glVertex2f(_w, _h-s);
	glVertex2f(_w, _h);
	glVertex2f(s, _h);
	glEnd();

	Unload();
}

