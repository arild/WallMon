#ifndef FONT_H_
#define FONT_H_

#include <string>
#include "FTGL/ftgl.h"


using namespace std;

class Font {
public:
	Font(int size, bool centerHorizontal=false, bool centerVertical=false);
	virtual ~Font();
	void RenderText(string text, float tx, float ty);
	void RenderText(string text, float tx, float ty, bool centerHorizontal, bool centerVertical);
private:
	int _fontSize;
	bool _centerHorizontal, _centerVertical;
	FTFont *_font;
};

#endif /* FONT_H_ */
