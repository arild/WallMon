/**
 * Implements a scene abstraction that allows entities to work and
 * operate on their own coordinate system.
 */

#include <GL/gl.h>
#include <boost/foreach.hpp>
#include <algorithm>
#include "Scene.h"

vector<Scene *> Scene::scenes;
Scene *Scene::current;

Scene::Scene(float x_, float y_, float w_, float h_, float virtualW_, float virtualH_) : x(x_), y(y_), w(w_), h(h_)
{
	scaleX = w_ / (float)virtualW_;
	scaleY = h_ / (float) virtualH_;
	scale = std::min(scaleX, scaleY);
	current = this;
}

Scene::~Scene()
{
}

void Scene::LoadVirtual()
{
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(scale, scale, 1);
}

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

/**
 * Takes coordinates according to scene coordinate system and returns all
 * entities within the scene that overlaps with given coordinates
 */
vector<Entity *> Scene::TestForEntityHits(float x, float y)
{
	LoadVirtual();
	vector<Entity *> v;
	for (int i = 0; i < entityList.size(); i++)
		if (entityList[i]->IsHit(x, y))
			v.push_back(entityList[i]);
	Unload();
	return v;
}

/**
 * Takes coordinates according to global coordinate system and returns
 * at maximum one scene that overlaps with given coordinates
 */
Scene *Scene::TestForSceneHit(float x, float y)
{
	BOOST_FOREACH(Scene *s, Scene::scenes)
	{
		if (x >= s->x && x <= s->x + s->w && y >= s->y && y <= s->y + s->h)
			return s;
	}
	return NULL;
}

void Scene::Run()
{
	current = this;
	for (int i = 0; i < entityList.size(); i++)
		entityList[i]->OnLoop();
	LoadVirtual();
	for (int i = 0; i < entityList.size(); i++)
		entityList[i]->OnRender();
	Unload();
	Visualize();
}

