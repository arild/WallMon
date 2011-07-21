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
	_itemsAll.push_back(item);
}

void NameTable::OnInit()
{
	_tsLastUpdate = 0;
	_font = new Font(FONT_SIZE);
	_fontLarge = new Font(FONT_SIZE + 1);
}

void NameTable::OnLoop()
{
	if (_PerformUpdate()) {
		scoped_lock lock(_mutex);
		sort(_itemsAll.begin(), _itemsAll.end(), TableItemCompare());
		_itemsDisplay = _GetTopRankedUniqueDisplayNameItems(10);
	}
}

void NameTable::OnRender()
{
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

	glColor3ub(255,255,255);
	_fontLarge->RenderText("Processes", 3, TABLE_ITEMS_TOP);

	for (int i = _current; i < _itemsDisplay.size(); i++) {
		if (y <= TABLE_ITEMS_BOTTOM)
			break;
		y -= TABLE_ITEM_HEIGHT;

		TableItem *item = _itemsDisplay[i];
		glColor3ub(item->r, item->g, item->b);
		if (i == _selected) {
			// Draw the rectagular marker for selected entry
			float y_ = y - TABLE_ITEM_HEIGHT / 4;

			glLineWidth(2);
			glBegin(GL_LINE_STRIP);
			glVertex3f(0, y_, 0);
			glVertex3f(30, y_, 0);
			glVertex3f(30, y_ + TABLE_ITEM_HEIGHT, 0);
			glVertex3f(0, y_ + TABLE_ITEM_HEIGHT, 0);
			glVertex3f(0, y_, 0);

			glEnd();
		}

		glBegin(GL_QUADS);
		glVertex2f(3, y);
		glVertex2f(6, y);
		glVertex2f(6, y + 3);
		glVertex2f(3, y + 3);
		glEnd();

		_font->RenderText(item->displayName, 9, y);
	}
}

vector<TableItem*> NameTable::_GetTopRankedUniqueDisplayNameItems(int numItems)
{
	vector<TableItem *> retval;
	for (int i = 0; i < _itemsAll.size() && retval.size() < numItems; i++) {
		if (_HasDisplayName(retval, _itemsAll[i]->displayName) == false)
			retval.push_back(_itemsAll[i]);
	}
	return retval;
}

bool NameTable::_HasDisplayName(vector<TableItem *> & items, string &displayName)
{
	for (int i = 0; i < items.size(); i++)
		if (items[i]->displayName.compare(displayName) == 0)
			return true;
	return false;
}

bool NameTable::_PerformUpdate()
{
	double tsCurrent = System::GetTimeInSec();
	if (tsCurrent - _tsLastUpdate > 2) {
		_tsLastUpdate = tsCurrent;
		return true;
	}
	return false;
}







