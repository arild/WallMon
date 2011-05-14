#ifndef NAMEDRAWER_H_
#define NAMEDRAWER_H_

#include <GL/gl.h>
#include <FTGL/ftgl.h>
#include "IEntity.h"
#include "NameTagList.h"

class LeftColumn : IEntity {
public:
	LeftColumn(NameTagList *nameTagList, FTFont *font);
	virtual ~LeftColumn();
	virtual void OnLoop();
	virtual void OnRender();
	virtual void OnCleanup();

private:
	NameTagList *_nameTagList;
	FTFont *_font;
	float _pos;
	void _DrawSymbolDescription();
	void _DrawProcessNames();
	float _GetPos();
	void _MovePos();
	void _MovePosLarge();
	void _ResetPos();
	void _DrawHeading(string text);

};
#endif /* NAMEDRAWER_H_ */
