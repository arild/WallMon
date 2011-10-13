/*
 * BoidDescription.cpp
 *
 *  Created on: May 29, 2011
 *      Author: arild
 */

#include "Button.h"
#include "BoidSharedContext.h"
#include "Scene.h"
#include "ControlPanel.h"
#include <glog/logging.h>
//#include "SOIL.h"
//#include "IL/il.h"

bool loadImage = false;
void ControlPanel::OnInit()
{
//	string path = "/home/arild/img_test.png";
//
//	ilInit();
//	ilLoadImage(path.c_str());
//	ILuint Width, Height;
//	width = ilGetInteger(IL_IMAGE_WIDTH);
//	height = ilGetInteger(IL_IMAGE_HEIGHT);
//
//	LOG(INFO) << "w=" << width << " | h=" << height;
//	ILubyte *data = ilGetData();
//
//	//	_tex2d = _GetTexture(path);
//	int id = 11;
//	glBindTexture(GL_TEXTURE_2D, id);
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	// Metric types
	Button *b = new Button(30, 235, 40, 40, QUAD);
	b->SetCallback(&CpuBoidButtonCallback);
	Scene::AddEntityCurrent((Entity *) b);

	b = new Button(30, 150, 40, 40, TRIANGLE_UP);
	b->SetCallback(&MemoryBoidButtonCallback);
	Scene::AddEntityCurrent((Entity *) b);

	b = new Button(30, 65, 40, 40, DIAMOND);
	b->SetCallback(&NetworkBoidButtonCallback);
	Scene::AddEntityCurrent((Entity *) b);

	b = new Button(30, -20, 40, 40, POLYGON);
	b->SetCallback(&StorageBoidButtonCallback);
	Scene::AddEntityCurrent((Entity *) b);

	_font = new Font(45, true, true);
}

void ControlPanel::OnLoop()
{
}

void ControlPanel::OnRender()
{
//	glBindTexture(GL_TEXTURE_2D, 11);
//	glBegin(GL_QUADS);
//	glTexCoord2f(0.0, 0.0);
//	glVertex3f(0.0, 0.0, 0.0);
//	glTexCoord2f(1.0, 0.0);
//	glVertex3f(100.0, 0.0, 0.0);
//	glTexCoord2f(1.0, 1.0);
//	glVertex3f(100.0, 100.0, 0.0);
//	glTexCoord2f(0.0, 1.0);
//	glVertex3f(0.0, 100.0, 0.0);
//	glEnd();
//	return;
	glColor3ub(0, 60, 0);
	_font->RenderText("CPU", 50, 290);
	_font->RenderText("Memory", 50, 205);
	_font->RenderText("Network", 50, 120);
	_font->RenderText("Storage", 50, 35);

}

void ControlPanel::OnCleanup()
{
}

void ControlPanel::Tap(float x, float y)
{

}

void ControlPanel::ScrollDown(float speed)
{

}

void ControlPanel::ScrollUp(float speed)
{

}

void ControlPanel::SwipeLeft(float speed)
{
	loadImage == true ? loadImage = false : loadImage = true;
}

void ControlPanel::SwipeRight(float speed)
{
	loadImage == true ? loadImage = false : loadImage = true;
}

void ControlPanel::CpuBoidButtonCallback()
{
	BoidSharedContext::showCpuBoid == true ? BoidSharedContext::showCpuBoid = false
			: BoidSharedContext::showCpuBoid = true;
}

void ControlPanel::MemoryBoidButtonCallback()
{
	BoidSharedContext::showMemoryBoid == true ? BoidSharedContext::showMemoryBoid = false
			: BoidSharedContext::showMemoryBoid = true;
}

void ControlPanel::NetworkBoidButtonCallback()
{
	BoidSharedContext::showIoInBoid == true ? BoidSharedContext::showIoInBoid = false
			: BoidSharedContext::showIoInBoid = true;
}

void ControlPanel::StorageBoidButtonCallback()
{
	BoidSharedContext::showIoOutBoid == true ? BoidSharedContext::showIoOutBoid = false
			: BoidSharedContext::showIoOutBoid = true;
}
