
#include <GL/gl.h>
#include "Entity.h"


bool Entity::IsHit(float x, float y)
{
	if (x >= tx && x <= tx + width && y >= ty && y <= ty + height)
		return true;
	return false;
}

void Entity::HandleHit(TouchEvent &event)
{

}

void EntityShape::DrawEntityShape()
{
	switch (entityShape)
	{
	case QUAD:
		glBegin(GL_QUADS);
		glVertex2f(tx - (width / 2), ty - (height / 2));
		glVertex2f(tx + (width / 2), ty - (height / 2));
		glVertex2f(tx + (width / 2), ty + (height / 2));
		glVertex2f(tx - (width / 2), ty + (height / 2));
		glEnd();
		break;
	case TRIANGLE:
		glBegin(GL_TRIANGLES);
		glVertex2f(tx - (width / 2), ty - (height / 2));
		glVertex2f(tx + (width / 2), ty - (height / 2));
		glVertex2f(tx, ty + (height / 2));
		glEnd();
		break;
	case DIAMOND:
		glBegin(GL_QUADS);
		glVertex2f(tx - (width / 2), ty);
		glVertex2f(tx, ty - (height / 2));
		glVertex2f(tx + (width / 2), ty);
		glVertex2f(tx, ty + (height / 2));
		glEnd();
		break;
	case POLYGON:
		glBegin(GL_QUADS);
		glVertex2f(tx - (width / 2), ty);
		glVertex2f(tx, ty - (height / 2));
		glVertex2f(tx + (width / 2), ty);
		glVertex2f(tx, ty + (height / 2));
		glEnd();
		break;
	}
}





