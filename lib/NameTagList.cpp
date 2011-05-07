/*
 * NameList.cpp
 *
 *  Created on: Apr 29, 2011
 *      Author: arild
 */

#include "NameTagList.h"

NameTag::NameTag(string _name, int _r, int _g, int _b)
{
	name = _name;
	r = _r;
	g = _g;
	b = _b;
}

NameTagList::NameTagList()
{
}

NameTagList::~NameTagList()
{
}

void NameTagList::Add(NameTag & item)
{
	scoped_lock lock(_mutex);
	if (_nameItems.size() == MAX_NUM_NAME_TAGS)
		return;
	if (_HasName(item.name))
		return;
	_nameItems.push_back(item);
}

vector<NameTag> NameTagList::GetList()
{
	scoped_lock lock(_mutex);
	return _nameItems;
}

bool NameTagList::_HasName(string name)
{
	for (int i = 0; i < _nameItems.size(); i++)
		if (_nameItems[i].name.compare(name) == 0)
			return true;
	return false;
}









