/**
 * Implements a scrollable and touchable table that holds and displays
 * named entries.
 */

#include <GL/gl.h>
#include <boost/foreach.hpp>
#include <glog/logging.h>
#include <sstream>
#include <algorithm>
#include "System.h"
#include "NameTable.h"

typedef boost::mutex::scoped_lock scoped_lock;

TableItem::TableItem(string displayName_, int r_, int g_, int b_)
{
	displayName = displayName_;
	r = r_;
	g = g_;
	b = b_;
	timestampMsec = System::GetTimeInMsec();
	score = 0;
}

NameTable::NameTable(int maxNumItemsToDisplay)
{
	_font = new Font(5);
	_maxNumItemsToDisplay = maxNumItemsToDisplay;
}

NameTable::~NameTable()
{
}

/**
 * Adds an item to the table
 *
 * It is assumed that the same item is not added more than once
 */
void NameTable::Add(TableItem *item)
{
	LOG(INFO) << "Addind item: " << item->displayName;
	scoped_lock lock(_mutex);
//	ItemContainerType::iterator it;
//	for (it = _items.begin(); it != _items.end(); it++) {
//		TableItem *current = *it;
//		if (item->score > current->score) {
//			_items.insert(it, item);
//			return;
//		}
//	}
	_items.push_back(item);
}

void NameTable::OnLoop()
{
	scoped_lock lock(_mutex);
	_items.sort(TableItemCompare());
}

void NameTable::OnRender()
{
	scoped_lock lock(_mutex);
	_DrawAllItems();
	glColor3ub(1, 0, 0);
	stringstream ss;
	_font->RenderText("TEST", 95, 10);
	_font->RenderText("aoeuaoeu", 70, 70);
}

void NameTable::OnCleanup()
{

}

void NameTable::HandleHit(TouchEvent & event)
{
}

void NameTable::_DrawAllItems()
{
	float s = 3;
	float y = 90;

	BOOST_FOREACH(TableItem *item, _items) {
		y -= s;

		glColor3ub(item->r, item->g, item->b);
		glBegin(GL_QUADS);
		glVertex2f(0, y);
		glVertex2f(s, y);
		glVertex2f(s, y + s);
		glVertex2f(0, y + s);
		glEnd();

		_font->RenderText(item->displayName, s * 2, y);

		y -= s;

		stringstream ss;
		ss << "Score: " << item->score;
		_font->RenderText(ss.str(), s * 2, y - s);

		y -= s*2;

		if (y <= 5)
			break;
	}
}

