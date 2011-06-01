
#include <GL/gl.h>
#include "Entity.h"
#include "Scene.h"

bool Entity::IsHit(float x, float y)
{
	if (x >= tx && x <= tx + width && y >= ty && y <= ty + height)
		return true;
	return false;
}

Entity::Entity()
{
	tx, ty, width, height = 0;
	Scene::current->entityList.push_back(this);
}

void Entity::HandleHit(TouchEvent &event)
{

}

EntityShape::EntityShape()
{
	centerShape = false;
}

void EntityShape::DrawEntityShape()
{
	float x, y;
	if (centerShape) {
		x = tx - (width / 2);
		y = ty - (height / 2);
	}
	else {
		x = tx;
		y = ty;
	}
	switch (entityShape)
	{
	case QUAD:
		glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x + width, y);
		glVertex2f(x + width, y + height);
		glVertex2f(x, y + height);
		glEnd();
		break;
	case TRIANGLE:
		glBegin(GL_TRIANGLES);
		glVertex2f(x, y);
		glVertex2f(x + width, y);
		glVertex2f(x + width/2, y + height);
		glEnd();
		break;
	case DIAMOND:
		glBegin(GL_QUADS);
		glVertex2f(x, y + height/2);
		glVertex2f(x + width/2, y);
		glVertex2f(x + width, y + height/2);
		glVertex2f(x + width/2, y + height);
		glEnd();
		break;
	case POLYGON:
		glBegin(GL_POLYGON);
		glVertex2f(x, y + height/4);
		glVertex2f(x + width/4, y);
		glVertex2f(x + width - width/4, y);
		glVertex2f(x + width, y + height/4);
		glVertex2f(x + width, y + height - height/4);
		glVertex2f(x + width - width/4, y + height);
		glVertex2f(x + width/4, y + height);
		glVertex2f(x, y + height - height/4);
		glEnd();
		break;
	}
}





