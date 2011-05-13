#ifndef FONT_H_
#define FONT_H_

#include <string>
#include "FTGL/ftgl.h"


using namespace std;

class Font {
public:
	Font();
	virtual ~Font();
	void RenderText(string text, int size, float tx, float ty, bool center=false);

private:
	FTFont *_font;
};

#endif /* FONT_H_ */
