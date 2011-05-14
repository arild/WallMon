/*
 * ControlPanel.cpp
 *
 *  Created on: May 13, 2011
 *      Author: arild
 */

#include "ControlPanel.h"
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>

float tailButton[] = {0, 0, 40, 40};
bool tailButtonPushed = false;

ControlPanel::ControlPanel()
{
}

ControlPanel::~ControlPanel()
{
}

void DrawCircle(int tx, int ty, int r, float lineWidth=1.0)
{
	float x, y;
	glLineWidth(lineWidth);
	glBegin(GL_LINES);
	for (int i = 0; i < 180; i++) {
		x = r * cos(i) - tx;
		y = r * sin(i) + ty;
		glVertex3f(x + ty, y - tx, 0);

		x = r * cos(i + 0.1) - tx;
		y = r * sin(i + 0.1) + ty;
		glVertex3f(x + ty, y - tx, 0);
	}
	glEnd();
}

void ControlPanel::TailButtonClick()
{
	if (tailButtonPushed)
		tailButtonPushed = false;
	else
		tailButtonPushed = true;
}

bool ControlPanel::IsTailButton()
{

}

void ControlPanel::OnLoop()
{
}

void ControlPanel::OnRender()
{
	glPushMatrix();
	if (tailButtonPushed)
		glColor3f(1, 0, 0);
	else
		glColor3f(0, 1, 0);
	DrawCircle(50, 50, 3);
	glPopMatrix();
}

void ControlPanel::OnCleanup()
{
}

//void ControlPanel::Handle(float x, float y)
//{
//	if (x >= tailButton[0] && x <= tailButton[2] && y >= tailButton[1] && y <= tailButton[3])
//		// hit
//		TailButtonClick();
//}


