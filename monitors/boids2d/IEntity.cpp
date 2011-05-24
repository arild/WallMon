
#include "IEntity.h"


bool IEntity::IsHit(float x, float y)
{
	if (x >= tx && x <= tx + width && y >= ty && y <= ty + height)
		return true;
	return false;
}

void IEntity::HandleHit(TouchEvent &event)
{

}



