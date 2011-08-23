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

Scene *Scene::current = NULL;


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
 * The virtual coordinate system has a virtual width and height, however,
 * its base point (bottom-left) is always in (0, 0).
 */
Scene::Scene(float x, float y, float w, float h, float virtualW, float virtualH) : _x(x), _y(y), _w(w), _h(h), _virtualW(virtualW), _virtualH(virtualH)
{
	float scaleX = w / (float)virtualW;
	float scaleY = h / (float) virtualH;
	_scale = std::min(scaleX, scaleY);
}

Scene::~Scene()
{
}

/**
 * Creates a scene within the given scene.
 *
 * The new sub-scene must have its "real" coordinates within the virtual width and
 * height of the given scene, otherwise the call will fail. The provided coordinates
 * defines a new virtual coordinate system on top of the already existing virtual coordinate system
 */
Scene *Scene::CreateSubScene(float x, float y, float w, float h, float virtualW, float virtualH)
{
	scoped_lock(_sceneMutex);
	CHECK(x >= 0 && x <= _virtualW && y >= 0 && y <= _virtualH);
	Scene *scene = new Scene(x, y, w, h, virtualW, virtualH);
	_subScenes.push_back(scene);
	return scene;
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

float Scene::GetScale()
{
	return _scale;
}

void Scene::RealToVirtualCoords(float realX, float realY, float *virtX, float *virtY)
{
	float localX = realX - _x;
	float localY = realY - _y;
	*virtX = localX * (1 / (float)_scale);
	*virtY = localY * (1 / (float)_scale);
}

void Scene::VirtualToRealCoords(float virtX, float virtY, float *realX, float *realY)
{
	CHECK(virtX >= 0 && virtX <= _virtualW && virtY >= 0 && virtY <= _virtualH);
	*realX = virtX * _scale;
	*realY = virtY * _scale;
}

/**
 * Draws a blue bounding box around the scene
 */
void Scene::Visualize()
{
	float s = 1 / _virtualW;
	float w = _virtualW;
	float h = _virtualH;

	glColor3f(0, 0, 1);

	// Left
	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(s, 0);
	glVertex2f(s, h);
	glVertex2f(0, h);
	glEnd();

	// Right
	glBegin(GL_QUADS);
	glVertex2f(w-s, 0);
	glVertex2f(w, 0);
	glVertex2f(w, h);
	glVertex2f(w-s, h);
	glEnd();

	// Bottom
	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(w, 0);
	glVertex2f(w, s);
	glVertex2f(0, s);
	glEnd();

	// Top
	glBegin(GL_QUADS);
	glVertex2f(0, h-s);
	glVertex2f(w, h-s);
	glVertex2f(w, h);
	glVertex2f(s, h);
	glEnd();
}

void Scene::AddEntity(Entity *entity)
{
	scoped_lock(_entityMutex);
	_entityListInit.push_back(entity);
}

void Scene::AddEntityCurrent(Entity *entity)
{
	if (current == NULL)
		return;
	current->_entityMutex.lock();
	current->_entityListInit.push_back(entity);
	current->_entityMutex.unlock();
}


/**
 * Takes coordinates according to scene coordinate system and returns all
 * entities within the scene that overlaps with given coordinates
 */
vector<EntityHit> Scene::TestForEntityHits(float x, float y)
{
	vector<EntityHit> result;
	if (_IsSceneHit(x, y) == false)
		return result;
	LOG(INFO) << "Scene hit";
	// In this scenario, we do not need to load the virtual scene, only provide
	// entities with coordinates in their virtual coordinate system
	RealToVirtualCoords(x, y, &x, &y);

	// Test for hits within scene
	_entityMutex.lock();
	for (int i = 0; i < _entityList.size(); i++)
		if (_entityList[i]->IsHit(x, y))
			result.push_back(EntityHit(x, y, this, _entityList[i]));
	_entityMutex.unlock();

	// Test for hits within sub-scenes
	_sceneMutex.lock();
	for (int i = 0; i < _subScenes.size(); i++) {
		vector<EntityHit> resultSubScene = _subScenes[i]->TestForEntityHits(x, y);
		// Combine results
		for (int j = 0; j < resultSubScene.size(); j++)
			result.push_back(resultSubScene[j]);
	}
	_sceneMutex.unlock();

	return result;
}

void Scene::Run()
{
	LoadVirtual();
	_RunEntities();
	_RunSubScenes();
	Unload();
}

void Scene::_RunEntities()
{
	scoped_lock(_entityMutex);
	current = this;
	while (_entityListInit.size() > 0) {
		Entity *e = _entityListInit.back();
		_entityListInit.pop_back();
		_entityList.push_back(e);
		e->OnInit();
	}
	for (int i = 0; i < _entityList.size(); i++)
		_entityList[i]->OnLoop();
	for (int i = 0; i < _entityList.size(); i++)
		_entityList[i]->OnRender();
}

void Scene::_RunSubScenes()
{
	scoped_lock(_sceneMutex);
	Entity::automaticallyAddToCurrentScene = true;
	for (int i = 0; i < Scene::_subScenes.size(); i++)
		Scene::_subScenes[i]->Run();
	Entity::automaticallyAddToCurrentScene = false;
}

bool Scene::_IsSceneHit(float x, float y)
{
	if (x >= _x && x <= _x + _w && y >= _y && y <= _y + _h)
		return true;
	return false;
}
