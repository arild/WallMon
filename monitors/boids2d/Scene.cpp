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
Scene::Scene(float x_, float y_, float w_, float h_, float virtualW_, float virtualH_) : x(x_), y(y_), w(w_), h(h_)
{
	float scaleX = w_ / (float)virtualW_;
	float scaleY = h_ / (float) virtualH_;
	scale = std::min(scaleX, scaleY);
}

Scene::~Scene()
{
}

/**
 * Loads the local coordinate system to the scene.
 * (0,0) is bottom-left of scene.
 */
void Scene::LoadVirtual()
{
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(scale, scale, 1);
}

/**
 * Loads the global coordinate system, however, (0,0) is still
 * bottom-left of current scene.
 */
void Scene::LoadReal()
{
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(1., 1., 1.);
}

void Scene::Unload()
{
	glPopMatrix();
}

void Scene::RealToVirtualCoords(float realX, float realY, float *virtX, float *virtY)
{
	float localX = realX - x;
	float localY = realY - y;
	*virtX = localX * (1 / (float)scale);
	*virtY = localY * (1 / (float)scale);
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

	Unload();
}

void Scene::AddEntity(Entity *entity)
{
	scoped_lock(_entityMutex);
	entityListInit.push_back(entity);
}

void Scene::AddEntityToCurrent(Entity *entity)
{
	if (current == NULL)
		return;
	current->_entityMutex.lock();
	current->entityListInit.push_back(entity);
	current->_entityMutex.unlock();
}

/**
 * Takes coordinates according to scene coordinate system and returns all
 * entities within the scene that overlaps with given coordinates
 */
vector<Entity *> Scene::TestForEntityHits(float x, float y)
{
	scoped_lock(_entityMutex);
	LoadVirtual();
	vector<Entity *> v;
	for (int i = 0; i < entityList.size(); i++)
		if (entityList[i]->IsHit(x, y))
			v.push_back(entityList[i]);
	Unload();
	return v;
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
//	Visualize();
}

void Scene::AddScene(Scene *scene)
{
	scoped_lock(_sceneMutex);
	scenes.push_back(scene);
}

/**
 * Takes coordinates according to global coordinate system and returns
 * at maximum one scene that overlaps with given coordinates
 *
 * Assumes that coordinates are within scene of the instantiated object
 */
Scene *Scene::TestForSceneHit(float x, float y)
{
	scoped_lock(_sceneMutex);
	BOOST_FOREACH(Scene *s, Scene::scenes)
	{
		if (x >= s->x && x <= s->x + s->w && y >= s->y && y <= s->y + s->h)
			return s;
	}
	return NULL;
}

void Scene::RunAllScenes()
{
	Entity::automaticallyAddToCurrentScene = true;
	scoped_lock(_sceneMutex);
	for (int i = 0; i < Scene::scenes.size(); i++)
		Scene::scenes[i]->Run();
	Entity::automaticallyAddToCurrentScene = false;
}


