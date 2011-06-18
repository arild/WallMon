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

const float TABLE_ITEMS_TOP = 85.;
const float TABLE_ITEMS_BOTTOM = 15.;
const float TABLE_ITEM_HEIGHT = 6.;
const float FONT_SIZE = 4.;

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
	_font = new Font(FONT_SIZE);
	_maxNumItemsToDisplay = maxNumItemsToDisplay;

	_current = 0;

	_selected = -1;
	// Set up hit box
	tx = 0;
	ty = TABLE_ITEMS_BOTTOM;
	width = 50;
	height = TABLE_ITEMS_TOP;
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
	scoped_lock lock(_mutex);
	_items.push_back(item);
}
void NameTable::OnLoop()
{
//	scoped_lock lock(_mutex);
//	_items.sort(TableItemCompare());
}

void NameTable::OnRender()
{
	scoped_lock lock(_mutex);
	_DrawAllItems();
}

void NameTable::OnCleanup()
{

}

void NameTable::HandleHit(TouchEvent & event)
{
	if (event.isDown == false) {
		// Find item to be visually marked
		int offset = (TABLE_ITEMS_TOP - event.y) / TABLE_ITEM_HEIGHT;
		_selected = _current + offset;
		return;
	}

	// Logic for sliding
	LOG(INFO) << "Radius: " << event.radius;
	LOG(INFO) << "Moved : " << event.movedDistance;
}

void NameTable::_DrawAllItems()
{
	float y = TABLE_ITEMS_TOP;

	for (int i = _current; i < _items.size(); i++) {
		if (y <= TABLE_ITEMS_BOTTOM)
			break;
		y -= TABLE_ITEM_HEIGHT;

		TableItem *item = _items[i];
		glColor3ub(item->r, item->g, item->b);
		if (i == _selected) {

			float y_ = y - (TABLE_ITEM_HEIGHT - FONT_SIZE) / 2;
			//y_ -= (_selected - _current) * TABLE_ITEM_HEIGHT;

			glLineWidth(2);
			glBegin(GL_LINE_STRIP);
			glVertex3f(0, y_, 0);
			glVertex3f(30, y_, 0);
			glVertex3f(30, y_ + TABLE_ITEM_HEIGHT, 0);
			glVertex3f(0, y_ + TABLE_ITEM_HEIGHT, 0);
			glEnd();
		}

		glBegin(GL_QUADS);
		glVertex2f(0, y);
		glVertex2f(3, y);
		glVertex2f(3, y + 3);
		glVertex2f(0, y + 3);
		glEnd();

		_font->RenderText(item->displayName, 6, y);
	}
}

