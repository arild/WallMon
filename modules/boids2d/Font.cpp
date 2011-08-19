/**
 * @file   Font.cpp
 * @Author Arild Nilsen
 * @date   May, 2011
 *
 * Provides a font render API that uses FTGL and takes the resolution of
 * the current scene into account in order to scale FTGL fonts correctly.
 *
 * Without this wrapper, the rendered text is likely to become blurry,
 * since it is automatically scaled according to some value set in glScalef().
 */

#include <GL/gl.h>
#include "Font.h"
#include "Config.h"
#include "Scene.h"
#include <glog/logging.h>

Font::Font(int size, bool centerHorizontal, bool centerVertical)
{
	string fontPath = Config::GetFontPath();
	_font = new FTTextureFont(fontPath.c_str());
	_font->FaceSize((unsigned int)(size * Scene::current->GetScale()));
	_centerHorizontal = centerHorizontal;
	_centerVertical = centerVertical;
}

Font::~Font()
{
	delete _font;
}

void Font::RenderText(string text, float tx, float ty)
{
	RenderText(text, tx, ty, _centerHorizontal, _centerVertical);
}

void Font::RenderText(string text, float tx, float ty, bool centerHorizontal, bool centerVertical)
{
	Scene *s = Scene::current;
	s->Unload();
	s->LoadReal();
	float horizontalAlignment = 0;
	if (centerHorizontal)
		horizontalAlignment = _font->Advance(text.c_str()) / (float)2;
	float verticalAlignment = 0;
	if (centerVertical)
		verticalAlignment = _font->FaceSize() / (float)2;
	LOG(INFO) << "TX: " << tx * s->GetScale();
	glTranslatef(tx * s->GetScale() - horizontalAlignment, ty * s->GetScale() - verticalAlignment, 0);
	_font->Render(text.c_str());
	s->Unload();
	s->LoadVirtual();
}





