/**
 * @file   Font.cpp
 * @Author Arild Nilsen
 * @date   May, 2011
 *
 * Provides a font render API that uses FTGL and takes the resolution of
 * the current scene into account in order to scale FTGL fonts correctly.
 */

#include <GL/gl.h>
#include "Font.h"
#include "Config.h"
#include "Scene.h"
#include <glog/logging.h>

#define STATIC_FONT_SIZE	50

Font::Font(int size, bool centerHorizontal, bool centerVertical)
{
	string fontPath = Config::GetFontPath();
	_font = new FTTextureFont(fontPath.c_str());
	_font->FaceSize(STATIC_FONT_SIZE);
	_fontSize = size;
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
	float actualFontSize = s->GetScale() * (float)_fontSize;
	float fontScale = (actualFontSize/(float)STATIC_FONT_SIZE) * 0.05;

	float horizontalAlignment = 0;
	if (centerHorizontal)
		horizontalAlignment = (_font->Advance(text.c_str()) * fontScale) / (float)2;
	float verticalAlignment = 0;
	if (centerVertical)
		verticalAlignment = (_font->FaceSize() * fontScale) / (float)2;

	glPushMatrix();
	glTranslatef(tx - horizontalAlignment, ty - verticalAlignment, 0);
	glScalef(fontScale, fontScale, 1.);
	_font->Render(text.c_str());
	glPopMatrix();
}




