/*
 * BoidDescription.cpp
 *
 *  Created on: May 29, 2011
 *      Author: arild
 */

#include "Button.h"
#include "BoidSharedContext.h"
#include "Scene.h"
#include "BoidDescription.h"


BoidDescription::BoidDescription()
{
	Button *b = new Button(5, 50, 20, 20);
	b->slowActivationApproach = true;
	b->SetCallback(&BoidDescription::CpuBoidButtonCallback);
	Scene::current->entityList.push_back((Entity *)b);

	b = new Button(30, 50, 20, 20);
	b->slowActivationApproach = true;
	b->SetCallback(&BoidDescription::MemoryBoidButtonCallback);
	Scene::current->entityList.push_back((Entity *)b);

	_font = new Font(6);
}

BoidDescription::~BoidDescription()
{
}

void BoidDescription::OnLoop()
{
}

void BoidDescription::OnRender()
{
	glColor3f(0, 0, 1);
	_font->RenderText("CPU", 15, 35, true);
	_font->RenderText("Memory", 40, 35, true);
}

void BoidDescription::OnCleanup()
{
}

void BoidDescription::CpuBoidButtonCallback()
{
	BoidSharedContext::showCpuBoid == true ? BoidSharedContext::showCpuBoid == false
			: BoidSharedContext::showCpuBoid == true;
}

void BoidDescription::MemoryBoidButtonCallback()
{
	BoidSharedContext::showMemoryBoid == true ? BoidSharedContext::showMemoryBoid == false
			: BoidSharedContext::showMemoryBoid == true;
}




