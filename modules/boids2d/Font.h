#ifndef FONT_H_
#define FONT_H_

#include <string>
#include "FTGL/ftgl.h"


using namespace std;

enum FONT_TYPE {
	FONT_TIMES,
	FONT_MONO
};

class Font {
public:
	Font(int size=5, bool centerHorizontal=false, bool centerVertical=false);
	virtual ~Font() {}

	static void Init(int displayWidth);
	static void Close();

	void SetFontType(FONT_TYPE fontType);
	void SetFontSize(int size);
	void SetAlignmentPolicy(bool centerHorizontal, bool centerVertical);
	void RenderText(string text, float tx, float ty);
	void RenderText(string text, float tx, float ty, bool centerHorizontal, bool centerVertical);
	float GetHorizontalPixelLength(string &text);
	float GetVerticalPixelLength(string &text);
	string TrimHorizontal(string text, int maxPixelLen);
	string TrimHorizontal(string text, int maxPixelLen, int n);
private:
	int _fontSize;
	bool _centerHorizontal, _centerVertical;
	FTFont *_font;
	static FTFont *_timesFont;
	static FTFont *_monoFont;
	static float _resoultionScaleFactor;
	float _GetFontScale();
};

#endif /* FONT_H_ */
