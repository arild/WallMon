#ifndef FONT_H_
#define FONT_H_

#include <string>
#include "FTGL/ftgl.h"


using namespace std;

class Font {
public:
	Font(int size);
	virtual ~Font();
	void RenderText(string text, float tx, float ty, bool center=false);

private:
	FTFont *_font;
};

#endif /* FONT_H_ */
