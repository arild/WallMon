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

FTFont *Font::_timesFont = NULL;
FTFont *Font::_monoFont = NULL;
float Font::_resoultionScaleFactor;

Font::Font(int size, bool centerHorizontal, bool centerVertical)
{
	_fontSize = size;
	_centerHorizontal = centerHorizontal;
	_centerVertical = centerVertical;
	_font = _timesFont;
}

/**
 * Create a common font for all entities. The size of this font is large
 * enough for all entities, and can be scaled down.
 */
void Font::Init(int displayWidth)
{
	_timesFont = new FTTextureFont(Config::GetTimesFontPath().c_str());
	_timesFont->FaceSize(STATIC_FONT_SIZE);
	_monoFont = new FTTextureFont(Config::GetMonoFontPath().c_str());
	_monoFont->FaceSize(STATIC_FONT_SIZE);
	displayWidth > 7000 ? _resoultionScaleFactor = 0.05 : _resoultionScaleFactor = 0.1;
}

void Font::Close()
{
	delete _timesFont;
	delete _monoFont;
}

void Font::SetFontType(FONT_TYPE fontType)
{
	switch (fontType)
	{
	case FONT_TIMES:
		_font = _timesFont;
		break;
	case FONT_MONO:
		_font = _monoFont;
		break;
	}
}

void Font::SetFontSize(int size)
{
	_fontSize = size;
}

void Font::SetAlignmentPolicy(bool centerHorizontal, bool centerVertical)
{
	_centerHorizontal = centerHorizontal;
	_centerVertical = centerVertical;
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

float Font::GetHorizontalPixelLength(string &text)
{
	return _font->Advance(text.c_str()) * _GetFontScale();
}

float Font::GetVerticalPixelLength(string &text)
{
	return _font->FaceSize() * _GetFontScale();
}

float Font::_GetFontScale()
{
	float actualFontSize = Scene::current->GetScale() * (float)_fontSize;
	return (actualFontSize/(float)STATIC_FONT_SIZE) * _resoultionScaleFactor;
}

string Font::TrimHorizontal(string text, int maxPixelLen)
{
	return TrimHorizontal(text, maxPixelLen, 0);
}

/**
 * Shortens/trims a string to a specified length, starting from
 * character n, counted backwards, and moving backwards.
 */
string Font::TrimHorizontal(string text, int maxPixelLen, int n)
{
	bool isTrimmed = false;
	n = text.length() - n;
	while (GetHorizontalPixelLength(text) > maxPixelLen) {
		text.erase(n--, 1);
		isTrimmed = true;
	}
	if (isTrimmed)
		text.replace(n, 1, ".");
	return text;
}
