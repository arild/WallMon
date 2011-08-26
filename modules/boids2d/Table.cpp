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
#include "Table.h"

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

void TableItem::AddRelatedBoid(BoidSharedContext *subItem)
{
	scoped_lock(_mutex);
	_relatedBoids.push_back(subItem);
}

vector<BoidSharedContext *> TableItem::GetRelatedBoids()
{
	scoped_lock(_mutex);
	return _relatedBoids;
}

Table::Table()
{
	_currentPixelIndex = 0;
	_selectedPixelIndex = -1;
	// Set up hit box
	tx = 0;
	ty = TABLE_BOTTOM;
	width = 50;
	height = TABLE_TOP;
}

Table::~Table()
{

}

/**
 * Adds an item to the table
 *
 * It is assumed that the same item is not added more than once
 */
void Table::Add(TableItem *item)
{
	scoped_lock lock(_mutex);
	_items.push_back(item);
}

void Table::OnInit()
{
	_tsLastUpdate = 0;
	_font = new Font(FONT_SIZE);
	_fontLarge = new Font(FONT_SIZE + 1);
}

void Table::OnLoop()
{
	if (_PerformUpdate()) {
		scoped_lock lock(_mutex);
		sort(_items.begin(), _items.end(), TableItemCompare());
	}
}

void Table::OnRender()
{
	_DrawAllItems();
	_DrawArrows();
}

void Table::OnCleanup()
{
}

void Table::Tap(float x, float y)
{
	LOG(INFO) << "Tap: x=" << x << " | y=" << y;
	// Find item to be visually marked
	_selectedPixelIndex = _currentPixelIndex + (100 - y);
//	LOG(INFO) << "selected index" << _selectedPixelIndex;

}

void Table::ScrollDown(float deltaY)
{
	LOG(INFO) << "SCROLL DOWN";
	_currentPixelIndex += 2;
	int maxPixelIndex = (_items.size() * 6) - 20;
	if (_currentPixelIndex > maxPixelIndex)
		_currentPixelIndex = maxPixelIndex;
}

void Table::ScrollUp(float deltaY)
{
	LOG(INFO) << "SCROLL UP";
	_currentPixelIndex -= 2;
	if (_currentPixelIndex < 0)
		_currentPixelIndex = 0;
}

void Table::_DrawAllItems()
{
	int startIndex = _currentPixelIndex / ITEM_HEIGHT;
	int selectedIndex = _selectedPixelIndex / ITEM_HEIGHT;
	float y = 100 - (_currentPixelIndex % (int)ITEM_HEIGHT);
	float offsetX = 8;
	for (int i = startIndex; i < _items.size(); i++) {
		if (y <= 0)
			break;
		y -= ITEM_HEIGHT;

		TableItem *item = _items[i];
		glColor3ub(item->r, item->g, item->b);
		if (i == selectedIndex) {
			// Draw the rectangular marker for selected entry
			float y_ = y - ITEM_HEIGHT / 4;
			glLineWidth(2);
			glBegin(GL_LINE_STRIP);
			glVertex2f(offsetX, y_);
			glVertex2f(offsetX + 35, y_);
			glVertex2f(offsetX + 35, y_ + ITEM_HEIGHT);
			glVertex2f(offsetX, y_ + ITEM_HEIGHT);
			glVertex2f(offsetX, y_);
			glEnd();
		}

		// Draw the rectangle in front of every entry
		glBegin(GL_QUADS);
		glVertex2f(offsetX + 3, y);
		glVertex2f(offsetX + 6, y);
		glVertex2f(offsetX + 6, y + 3);
		glVertex2f(offsetX + 3, y + 3);
		glEnd();

		_font->RenderText(item->displayName, offsetX + 9, y);
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
	_fontLarge->RenderText("Processes", offsetX + 3, TABLE_TOP + 8);
}

vector<TableItem*> Table::_GetTopRankedUniqueDisplayNameItems(int numItems)
{
	vector<TableItem *> retval;
	for (int i = 0; i < _items.size() && retval.size() < numItems; i++) {
		if (_HasDisplayName(retval, _items[i]->displayName) == false)
			retval.push_back(_items[i]);
	}
	return retval;
}

bool Table::_HasDisplayName(vector<TableItem *> & items, string &displayName)
{
	for (int i = 0; i < items.size(); i++)
		if (items[i]->displayName.compare(displayName) == 0)
			return true;
	return false;
}

bool Table::_PerformUpdate()
{
	double tsCurrent = System::GetTimeInSec();
	if (tsCurrent - _tsLastUpdate > 10) {
		_tsLastUpdate = tsCurrent;
		return true;
	}
	return false;
}

void Table::_DrawArrows()
{
	glColor3ub(0, 255, 0);

	glBegin(GL_QUADS);
	glVertex2f(4, 55);
	glVertex2f(5, 55);
	glVertex2f(5, 60);
	glVertex2f(4, 60);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex2f(3, 60);
	glVertex2f(6, 60);
	glVertex2f(4.5, 63);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(4, 40);
	glVertex2f(5, 40);
	glVertex2f(5, 45);
	glVertex2f(4, 45);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex2f(3, 40);
	glVertex2f(6, 40);
	glVertex2f(4.5, 37);
	glEnd();

	glColor3ub(255, 0, 0);

	stringstream up, down;
	int numUp = _currentPixelIndex / ITEM_HEIGHT;
	up << numUp;
	_font->RenderText(up.str(), 4.5, 68, true, true);
	int numDown = _items.size() - numUp - (70 / ITEM_HEIGHT);
	down << numDown;
	_font->RenderText(down.str(), 4.5, 33, true, true);
}







