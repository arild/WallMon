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
#include "Config.h"
#include <glog/logging.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"

GLuint load_texture(const char* file)
{
	SDL_Surface* surface = IMG_Load(file);
	GLuint texture;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	SDL_PixelFormat *format = surface->format;
	if (format->Amask) {
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, surface->w, surface->h, GL_RGBA, GL_UNSIGNED_BYTE,
				surface->pixels);
	} else {
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, surface->w, surface->h, GL_RGB, GL_UNSIGNED_BYTE,
				surface->pixels);
	}
	SDL_FreeSurface(surface);
	return texture;
}

void ControlPanel::OnInit()
{
	_showLegend = false;
	_imageIsLoaded = false;
}

void ControlPanel::OnLoop()
{
}

void ControlPanel::OnRender()
{
	if (_showLegend == false)
		return;
	if (_imageIsLoaded == false) {
		_imageIsLoaded = true;
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glActiveTexture(1);
		_textureId = load_texture(Config::GetInstructionManualPath().c_str());
		glActiveTexture(0);
		glPopAttrib();
	}

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glActiveTexture(1);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _textureId);

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glRotatef(180.0f, 1.0f, 0, 0);

	glBegin(GL_QUADS);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0, 0.0, 0.0);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(100.0, 0.0, 0.0);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(100.0, 100.0, 0.0);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0, 100.0, 0.0);
	glEnd();
	glPopMatrix();

	glActiveTexture(0);
	glPopAttrib();
}

void ControlPanel::OnCleanup()
{
}

void ControlPanel::Tap(float x, float y)
{
	Clap();
}

void ControlPanel::Clap()
{
	_showLegend = !_showLegend;
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
