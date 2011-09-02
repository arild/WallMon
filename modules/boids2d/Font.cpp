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

#define STATIC_FONT_SIZE	75

FTFont *Font::_font = NULL;

Font::Font(int size, bool centerHorizontal, bool centerVertical)
{
	_fontSize = size;
	_centerHorizontal = centerHorizontal;
	_centerVertical = centerVertical;
}

/**
 * Create a common font for all entities. The size of this font is large
 * enough for all entities, and can be scaled down.
 */
void Font::Init()
{
	string fontPath = Config::GetFontPath();
	_font = new FTTextureFont(fontPath.c_str());
	_font->FaceSize(STATIC_FONT_SIZE);
}

void Font::Close()
{
	delete _font;
}

void Font::RenderText(string text, float tx, float ty)
{
	RenderText(text, tx, ty, _centerHorizontal, _centerVertical);
}

void Font::RenderText(string text, float tx, float ty, bool centerHorizontal, bool centerVertical)
{
	float horizontalAlignment = 0;
	if (centerHorizontal)
		horizontalAlignment = GetHorizontalPixelLength(text) / (float)2;
	float verticalAlignment = 0;
	if (centerVertical)
		verticalAlignment = GetVerticalPixelLength(text) / (float)2;

	glPushMatrix();
	glTranslatef(tx - horizontalAlignment, ty - verticalAlignment, 0);
	float fontScale = _GetFontScale();
	glScalef(fontScale, fontScale, 1.);
	_font->Render(text.c_str());
	glPopMatrix();
}

int Font::GetHorizontalPixelLength(string &text)
{
	return _font->Advance(text.c_str()) * _GetFontScale();
}

int Font::GetVerticalPixelLength(string &text)
{
	return _font->FaceSize() * _GetFontScale();
}

float Font::_GetFontScale()
{
	float actualFontSize = Scene::current->GetScale() * (float)_fontSize;
	return actualFontSize/(float)STATIC_FONT_SIZE * 0.05;
}

string Font::TrimHorizontal(string text, int maxPixelLen)
{
	bool isTrimmed = false;
	while (GetHorizontalPixelLength(text) > maxPixelLen) {
		text.erase(text.length() - 2, 1);
		isTrimmed = true;
	}
	if (isTrimmed)
		text += ".";
	return text;
}

