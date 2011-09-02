#ifndef FONT_H_
#define FONT_H_

#include <string>
#include "FTGL/ftgl.h"


using namespace std;

class Font {
public:
	Font(int size, bool centerHorizontal=false, bool centerVertical=false);
	virtual ~Font() {}

	static void Init();
	static void Close();

	void RenderText(string text, float tx, float ty);
	void RenderText(string text, float tx, float ty, bool centerHorizontal, bool centerVertical);
	int GetHorizontalPixelLength(string &text);
	int GetVerticalPixelLength(string &text);
	string TrimHorizontal(string text, int maxPixelLen);
private:
	int _fontSize;
	bool _centerHorizontal, _centerVertical;
	static FTFont *_font;
	float _GetFontScale();
};

#endif /* FONT_H_ */
