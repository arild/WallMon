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
#include "IL/il.h"
#include "SDL/SDL.h"

bool loadImage = false;
GLuint id;
//unsigned char* data;

void ControlPanel::OnInit()
{
	string path = "/home/arild/test.BMP";

	GLuint texture; // This is a handle to our texture object
	SDL_Surface *surface; // This surface will tell us the details of the image
	GLenum texture_format;
	GLint nOfColors;

	if ((surface = SDL_LoadBMP(path.c_str()))) {

		// Check that the image's width is a power of 2
		if ((surface->w & (surface->w - 1)) != 0) {
			LOG(INFO) << "warning: image.bmp's width is not a power of 2";
		}

		// Also check if the height is a power of 2
		if ((surface->h & (surface->h - 1)) != 0) {
			LOG(INFO) << "warning: image.bmp's height is not a power of 2";
		}

		// get the number of channels in the SDL surface
		nOfColors = surface->format->BytesPerPixel;
		if (nOfColors == 4) // contains an alpha channel
		{
			if (surface->format->Rmask == 0x000000ff)
				texture_format = GL_RGBA;
			else
				texture_format = GL_BGRA;
		} else if (nOfColors == 3) // no alpha channel
		{
			if (surface->format->Rmask == 0x000000ff)
				texture_format = GL_RGB;
			else
				texture_format = GL_BGR;
		} else {
			LOG(INFO) << "warning: the image is not truecolor..  this will probably break";
			// this error should not go unhandled
		}

		glEnable(GL_TEXTURE_2D);

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);


		LOG(INFO) << "id: " << id;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);

		LOG(INFO) << "before";
		LOG(INFO) << gluErrorString(glGetError());


		// Have OpenGL generate a texture object handle for us
//		glGenTextures(1, &id);
//
//		// Bind the texture object
//		glBindTexture(GL_TEXTURE_2D, id);
//
//		// Set the texture's stretching properties
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//		// Edit the texture object's image data using the information SDL_Surface gives us
//		glTexImage2D(GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0, texture_format,
//				GL_UNSIGNED_BYTE, surface->pixels);
	} else {
		LOG(INFO) << "SDL could not load image.bmp: %s\n" << SDL_GetError();
	}

	//	ilInit();
	//	ilLoadImage(path.c_str());
	//	width  = ilGetInteger(IL_IMAGE_WIDTH);
	//	height = ilGetInteger(IL_IMAGE_HEIGHT);
	//	LOG(INFO) << "w=" << width << " | h=" << height;
	//	ILubyte *data = ilGetData();

	/*
	 width  = 512;
	 height = 512;

	 data = (unsigned char*)malloc(width * height * 3);

	 */
	//	for (int i=0; i < width * height * 3; i+=3)
	//	{
	//		data[i]   = (unsigned char)i; //(unsigned char)rand() % 255;
	//		data[i+1] = (unsigned char)i;
	//		data[i+2] = (unsigned char)i;
	//	}
	/*
	 LOG(INFO) << width * height * 4;

	 //	_tex2d = _GetTexture(path);
	 */

	//glActiveTextureARB(2);


	return;
	// Metric types
	Button *b = new Button(30, 235, 30, 30, QUAD);
	b->SetCallback(&CpuBoidButtonCallback);
	Scene::AddEntityCurrent((Entity *) b);

	b = new Button(30, 150, 30, 30, TRIANGLE_UP);
	b->SetCallback(&MemoryBoidButtonCallback);
	Scene::AddEntityCurrent((Entity *) b);

	b = new Button(30, 65, 30, 30, DIAMOND);
	b->SetCallback(&NetworkBoidButtonCallback);
	Scene::AddEntityCurrent((Entity *) b);

	b = new Button(30, -20, 30, 30, POLYGON);
	b->SetCallback(&StorageBoidButtonCallback);
	Scene::AddEntityCurrent((Entity *) b);

	_font = new Font(35, true, true);
}

void ControlPanel::OnLoop()
{
}

void ControlPanel::OnRender()
{
	//glActiveTextureARB(2);
	glEnable(GL_TEXTURE_2D);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, id);

	LOG(INFO) << "id2: " << id;

	glBegin(GL_QUADS);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0, 0.0, 0.0);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(100.0, 0.0, 0.0);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(100.0, 100.0, 0.0);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0, 100.0, 0.0);

	glEnd();

	//glActiveTextureARB(0);
	//glDisable(GL_TEXTURE_2D);

	return;

	glColor3ub(0, 200, 0);
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
