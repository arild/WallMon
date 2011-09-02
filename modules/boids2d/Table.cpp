/**
 * Implements a scrollable and touchable table that holds and displays
 * named entries. The implementation is highly general, however, tailored
 * for having process names in the higher-level table, while specific processes
 * in the lower-level table.
 */

#include <GL/gl.h>
#include <boost/foreach.hpp>
#include <glog/logging.h>
#include <sstream>
#include <algorithm>
#include "System.h"
#include "Table.h"
#include "Scene.h"

typedef boost::mutex::scoped_lock scoped_lock;

const float TABLE_TOP = 85.;
const float TABLE_BOTTOM = 15.;
const float ITEM_HEIGHT = 6.;
const float FONT_SIZE = 4.;

TableItem::TableItem(string displayName_, int r_, int g_, int b_)
{
	key = displayName_;
	r = r_;
	g = g_;
	b = b_;
	score = 0;
}

void TableItem::AddBoid(BoidSharedContext *subItem)
{
	scoped_lock(_mutex);
	_subItems.push_back(subItem);
}

vector<BoidSharedContext *> TableItem::GetBoids()
{
	scoped_lock(_mutex);
	return _subItems;
}

Table::Table(bool isTopLevelTable)
{
	_isTopLevelTable = isTopLevelTable;
}

Table::~Table()
{
	delete _subTable;
}

/**
 * Adds an item to the table. It is assumed that the same
 * item is not added more than once.
 *
 * The hard-coded policy for grouping items are as follows:
 * - The top-level table groups related items (same key) together,
 *   and stores all specific items as sub-items
 * - The bottom-level table does no grouping
 */
void Table::Add(TableItem *item)
{
	scoped_lock(_mutex);
	vector<TableItem *> *itemGroup = _GetItemGroup_NoLock(item->key);
	if (itemGroup == NULL || !_isTopLevelTable) {
		// Group not present, create it
		vector<TableItem *> v;
		v.push_back(item);
		_items.push_back(v);
	}
	else
		itemGroup->push_back(item);
}

void Table::Add(vector<TableItem *> items)
{
	for (int i = 0; i < items.size(); i++)
		Add(items[i]);
}

void Table::Clear()
{
	scoped_lock(_mutex);
	_items.clear();
}

void Table::OnInit()
{
	_currentPixelIndex = 0;
	_selectedPixelIndex = -1;

	// Set up hit box
	tx = 0;
	ty = TABLE_BOTTOM;
	width = 50;
	height = TABLE_TOP;

	if (_isTopLevelTable) {
		// Create and setup hitbox for the sub-table
		// Note: Entities created by other entities are automatically
		// added to current scene
		_subTable = new Table(false);
		_subTable->tx = 50;
		_subTable->ty = 0;
		_subTable->width = 50;
		_subTable->height = 100;
	}

	_tsLastUpdate = 0;
	_font = new Font(FONT_SIZE);
	_fontLarge = new Font(FONT_SIZE + 1);
}

void Table::OnLoop()
{
//	if (_PerformUpdate()) {
//		scoped_lock lock(_mutex);
//		sort(_items.begin(), _items.end(), TableGroupCompare());
//		_selectedPixelIndex = -1;
//	}
}

void Table::OnRender()
{
	if (!_isTopLevelTable)
		glTranslatef(50, 0, 0);
	_DrawAllItems();
//	_DrawArrows();
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

	if (_isTopLevelTable) {
		int idx = _SelectedPixelToItemIndex();
		vector<TableItem *> group = _items[idx];
		_subTable->Clear();
		_subTable->Add(group);
	}
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
	int startIndex = _CurrentPixelToItemIndex();
	int selectedIndex = _SelectedPixelToItemIndex();
	float y = _GetStartPixel();

	float markerStartPixel;
	TableItem *markedItem = NULL;

	for (int i = startIndex; i < _items.size(); i++) {
		if (y <= 0)
			break;
		y -= ITEM_HEIGHT;

		TableItem *item = _items[i][0];
		glColor3ub(item->r, item->g, item->b);
		if (i == selectedIndex) {
			markerStartPixel = y - ITEM_HEIGHT / 4;
			markedItem = _items[i][0];
		}

		// Draw the rectangle in front of every entry
		glBegin(GL_QUADS);
		glVertex2f(6, y);
		glVertex2f(9, y);
		glVertex2f(9, y + 3);
		glVertex2f(6, y + 3);
		glEnd();

		string s = _font->TrimHorizontal(item->key, 30);
		_font->RenderText(s, 12, y);
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

	// Draw the rectangular marker for selected entry
	if (markedItem != NULL) {
		glColor3ub(markedItem->r, markedItem->g, markedItem->b);
		y = markerStartPixel;
		string s = _font->TrimHorizontal(markedItem->key, 30);
		float w = _font->GetHorizontalPixelLength(s);
		glLineWidth(2);
		glBegin(GL_LINE_STRIP);
		glVertex2f(3, y);
		glVertex2f(15 + w, y);
		glVertex2f(15 + w, y + ITEM_HEIGHT);
		glVertex2f(3, y + ITEM_HEIGHT);
		glVertex2f(3, y);
		glEnd();
	}

	glColor3ub(255,255,255);
	string heading = "Process Id";
	if (_isTopLevelTable)
		heading = "Process Name";
	_fontLarge->RenderText(heading, 3, TABLE_TOP + 8);
}

vector<TableItem *> *Table::_GetItemGroup_NoLock(string &itemKey)
{
	for (int i = 0; i < _items.size(); i++)
		if (_items[i][0]->key.compare(itemKey) == 0)
			return &_items[i];
	return NULL;
}

bool Table::_PerformUpdate()
{
	double tsCurrent = System::GetTimeInSec();
	if (tsCurrent - _tsLastUpdate > 3) {
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

int Table::_SelectedPixelToItemIndex()
{
	if (_selectedPixelIndex == -1)
		return -1;
	return _selectedPixelIndex / ITEM_HEIGHT;
}

int Table::_CurrentPixelToItemIndex()
{
	return _currentPixelIndex / ITEM_HEIGHT;
}

int Table::_GetStartPixel()
{
	return 100 - (_currentPixelIndex % (int)ITEM_HEIGHT);
}

