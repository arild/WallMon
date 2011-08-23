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

const float TABLE_TOP = 85.;
const float TABLE_BOTTOM = 15.;
const float ITEM_HEIGHT = 6.;
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

NameTable::NameTable()
{
	_currentPixelIndex = 0;
	_selectedPixelIndex = -1;
	// Set up hit box
	tx = 0;
	ty = TABLE_BOTTOM;
	width = 50;
	height = TABLE_TOP;
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
	if (_PerformUpdate() && false) {
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

void NameTable::Up(float x, float y)
{
	LOG(INFO) << "IS UP: x=" << x << " | y=" << y;
	// Find item to be visually marked
	_selectedPixelIndex = _currentPixelIndex + (100 - y);
	LOG(INFO) << "selected index" << _selectedPixelIndex;

}

void NameTable::_DrawAllItems()
{
	float y = 100;

	int startIndex = _currentPixelIndex / ITEM_HEIGHT;
	int selectedIndex = _selectedPixelIndex / ITEM_HEIGHT;
	for (int i = startIndex; i < _itemsAll.size(); i++) {
		if (y <= 0)
			break;
		y -= ITEM_HEIGHT;

		TableItem *item = _itemsAll[i];
		glColor3ub(item->r, item->g, item->b);
		if (i == selectedIndex) {
			// Draw the rectangular marker for selected entry
			float y_ = y - ITEM_HEIGHT / 4;

			glLineWidth(2);
			glBegin(GL_LINE_STRIP);
			glVertex2f(3, y_);
			glVertex2f(35, y_);
			glVertex2f(35, y_ + ITEM_HEIGHT);
			glVertex2f(3, y_ + ITEM_HEIGHT);
			glVertex2f(3, y_);

			glEnd();
		}

		// Draw the rectangle in front of every entry
		glBegin(GL_QUADS);
		glVertex2f(6, y);
		glVertex2f(9, y);
		glVertex2f(9, y + 3);
		glVertex2f(6, y + 3);
		glEnd();

		_font->RenderText(item->displayName, 12, y);
	}

	// Black out the top and bottom which is not part of the table
	glColor3ub(0,0,0);
	glBegin(GL_QUADS);
	glVertex2f(0, TABLE_TOP);
	glVertex2f(50, TABLE_TOP);
	glVertex2f(50, 100);
	glVertex2f(0, 100);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(50, 0);
	glVertex2f(50, TABLE_BOTTOM);
	glVertex2f(0, TABLE_BOTTOM);
	glEnd();

	glColor3ub(255,255,255);
	_fontLarge->RenderText("Processes", 6, TABLE_TOP + 8);

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



