/*
 * Scene.cpp
 *
 *  Created on: Apr 30, 2011
 *      Author: arild
 */

#include <GL/gl.h>
#include "Scene.h"
#include <algorithm>


Scene::Scene(float x_, float y_, float w_, float h_, float virtualW_, float virtualH_)// : x(x_), y(y_), w(w_), h(h_)
{
	x = x_;
	y = y_;
	w = w_;
	h = h_;
	float scaleX = w_ / (float)virtualW_;
	float scaleY = h_ / (float) virtualH_;
	scale = std::min(scaleX, scaleY);
}

Scene::~Scene()
{
}

void Scene::Load()
{
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(scale, scale, 1);
}

void Scene::Unload()
{
	glPopMatrix();
}

//void Scene::ScreenToSceneCoords(float screenX, float screenY, float *sceneX, float *sceneY)
//{
//	*sceneX = (screenX / (float) SCREEN_WIDTH) * (y - x);
//	*sceneY = (screenY / (float) SCREEN_HEIGHT) * (h - w);
//}
//
//void Scene::SceneToScreenCoords(float sceneX, float sceneY, float *screenX, float *screenY)
//{
//	float rx, ry;
//	SceneToRelativeCoords(sceneX, sceneY, &rx, &ry);
//
//	float scaleFactorX = (float) SCREEN_WIDTH / (float) 100;
//	float scaleFactorY = (float) SCREEN_HEIGHT / (float) 100;
//
//	*screenX = rx * scaleFactorX;
//	*screenY = ry * scaleFactorY;
//
//}
//
//void Scene::SceneToRelativeCoords(float sx, float sy, float *rx, float *ry)
//{
//	float scaleFactorX = (y - x) / (float) 100;
//	float scaleFactorY = (h - w) / (float) 100;
//	*rx = x + (sx * scaleFactorX);
//	*ry = w + (sy * scaleFactorY);
//}

