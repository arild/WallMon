/*
 * NameDrawer.h
 *
 *  Created on: May 1, 2011
 *      Author: arild
 */

#ifndef NAMEDRAWER_H_
#define NAMEDRAWER_H_

#include <GL/gl.h>
#include <FTGL/ftgl.h>
#include "Entity.h"
#include "NameTagList.h"

class NameDrawer : Entity {
public:
	NameDrawer(NameTagList *nameTagList, FTFont *font);
	virtual ~NameDrawer();
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
