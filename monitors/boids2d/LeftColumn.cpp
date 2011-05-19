/**
 * @file   LeftColumn.cpp
 * @Author Arild Nilsen
 * @date   May, 2011
 *
 * Entity that draws all information on the left side of the screen,
 * including the name tag list
 */

#include <glog/logging.h>
#include "LeftColumn.h"

#define W	1024
#define H 768*4

LeftColumn::LeftColumn(NameTagList *nameTagList, FTFont *font)
{
	_nameTagList = nameTagList;
	_font = font;
	_font->FaceSize(100);
}

LeftColumn::~LeftColumn()
{
}

void LeftColumn::OnLoop()
{
}

void LeftColumn::OnRender()
{
	_ResetPos();
	_DrawSymbolDescription();
	_DrawProcessNames();
}

void LeftColumn::OnCleanup()
{
}

void LeftColumn::_DrawSymbolDescription()
{
	_MovePosLarge();
	//_DrawHeading("Symbol Description");
	_MovePosLarge();

	float s = 50;
	float x;
	float y = _GetPos();
//
//	glColor3ub(0, 255, 0);
//	glBegin(GL_QUADS);
//	glVertex2f(0, y);
//	glVertex2f(s, y);
//	glVertex2f(s, y+s);
//	glVertex2f(0, y+s);
//	glEnd();
//
//	x += s * 2;
//	glPushMatrix();
//	glTranslatef(x, y, 0);
//	_font->Render("CPU");
//	glPopMatrix();
}

void LeftColumn::_DrawProcessNames()
{
	vector<NameTag> v = _nameTagList->GetList();

	float s = 50;
	float y;

	_MovePosLarge();
	_DrawHeading("Process Names");
	_MovePosLarge();
	for (int i = 0; i < v.size(); i++) {
		y = _GetPos();
		glPushMatrix();
		glTranslatef(s*2, y, 0);
		glColor3ub(v[i].r, v[i].g, v[i].b);
		_font->Render(v[i].name.c_str());
		glPopMatrix();

		glBegin(GL_QUADS);
		glVertex2f(0, y);
		glVertex2f(s, y);
		glVertex2f(s, y+s);
		glVertex2f(0, y+s);
		glEnd();
		_MovePos();
	}
}

float LeftColumn::_GetPos()
{
	return _pos;
}

void LeftColumn::_MovePos()
{
	_pos -= 100;
}

void LeftColumn::_MovePosLarge()
{
	_pos -= 150;
}

void LeftColumn::_ResetPos()
{
	_pos = H - 50;
}


void LeftColumn::_DrawHeading(string text)
{
	float y = _GetPos();
	glPushMatrix();
	glTranslatef(0, y, 0);
	glColor3ub(255, 255, 255);
	_font->Render(text.c_str());
	glBegin(GL_QUADS);
	glVertex2f(0, y-40);
	glVertex2f(text.length() * 50, y-40);
	glVertex2f(text.length() * 50, y-35);
	glVertex2f(0, y-35);
	glEnd();
	glPopMatrix();
}

