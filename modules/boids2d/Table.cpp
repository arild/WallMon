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
#include "Button.h"

typedef boost::mutex::scoped_lock scoped_lock;

float TABLE_TOP = 85.;
float TABLE_BOTTOM = 15.;
float ITEM_HEIGHT = 6.;
float FONT_SIZE = 4.;

TableItem::TableItem(string key_)
{
	key = key_;
	score = 0;
}

void TableItem::AddBoid(BoidSharedContext *boid)
{
	scoped_lock(_mutex);
	if (_boids.size() == 0) {
		r = boid->red;
		g = boid->green;
		b = boid->blue;
	}
	_boids.push_back(boid);
}

vector<BoidSharedContext *> TableItem::GetBoids()
{
	scoped_lock(_mutex);
	return _boids;
}

struct TableGroupCompareAlphabetically {
	bool operator()(const vector<TableItem *> a, const vector<TableItem *> b)
	{
		if (ToLower(a[0]->key).compare(ToLower(b[0]->key)) < 0)
			return true;
		return false;
	}

	string ToLower(string &s)
	{
		for (int i = 0; i < s.length(); i++)
			s[i] = tolower(s[i]);
		return s;
	}
};

struct TableGroupCompareUtilization {
	bool operator()(const vector<TableItem *> a, const vector<TableItem *> b)
	{
		return a[0]->score > b[0]->score;
	}
};


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
	if (itemGroup == NULL) {
		// Group not present, create it
		vector<TableItem *> v;
		v.push_back(item);
		_items.push_back(v);
	} else
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
	_selectedItem = NULL;
	// Set up hit box

	if (_isTopLevelTable) {
		tx = 0;
		ty = TABLE_BOTTOM;
		width = 50;
		height = TABLE_TOP;

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
	_fontSub = new Font(FONT_SIZE - 1);
	_fontSub->SetFontType(FONT_MONO);
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
	if (_isTopLevelTable)
		//ITEM_HEIGHT = 6;
		;
	else {
		glTranslatef(50, 0, 0);
		//ITEM_HEIGHT = 16;
	}
	_DrawAllItems();
//	_DrawArrows();
}

void Table::OnCleanup()
{
}

void Table::Tap(float x, float y)
{
	if (y > TABLE_TOP || y < TABLE_BOTTOM)
		return;
	int idx = _RelativePixelToItemIndex(y);
	if (idx < 0 || idx >= _items.size())
		return;
	TableItem *selectedItem = _items[idx][0];

	if (_isTopLevelTable) {
		// Populate the sub-table with all items in selected group
		vector<TableItem *> group = _items[idx];
		_subTable->Clear();
		if (selectedItem == _selectedItem) {
			// Unhighlight
			_selectedItem = NULL;
		}
		else {
			_selectedItem = selectedItem;
			_subTable->Add(group);
			_HighlightBoids(group);
		}
	}
}

void Table::ScrollDown(float speed)
{
//	LOG(INFO) << "TABLE SCROLL DOWN";
	_currentPixelIndex += speed;
	int maxPixelIndex = (_items.size() * ITEM_HEIGHT) - 35;
	_currentPixelIndex = fmin(_currentPixelIndex, (float)maxPixelIndex);
}

void Table::ScrollUp(float speed)
{
	_currentPixelIndex -= speed;
	_currentPixelIndex = fmax(_currentPixelIndex, -35.);
}

void Table::SwipeLeft(float speed)
{
	_SortTableAlphabetically();
}

void Table::SwipeRight(float speed)
{
	_SortTableUtilization();
}

void Table::_DrawAllItems()
{
	float selectBoxStartPixel;
	int startIndex = _CurrentPixelToItemIndex();
	for (int i = 0; i < 20 ; i++) {
		int itemIndex = startIndex + i;
		if (itemIndex < 0 || itemIndex >= _items.size())
			continue;
		float y = _ItemNumberToPixel(i);
		if (y < 0)
			break;

		// Render first item of group. For the sub-table, there is only
		// groups containing one item, hence all items in the sub-table are rendered
		TableItem *item = _items[itemIndex][0];
		glColor3ub(item->r, item->g, item->b);

		if (_isTopLevelTable) {
			// Draw the rectangle in front of every entry
			y -= 4.5;
			glBegin(GL_QUADS);
			glVertex2f(6, y);
			glVertex2f(9, y);
			glVertex2f(9, y + 3);
			glVertex2f(6, y + 3);
			glEnd();

			int groupSize = _items[itemIndex].size();
			stringstream ss;
			ss << groupSize;
			string s = item->procName + " (" + ss.str() + ")";
			s = _font->TrimHorizontal(s, 30, 4 +  ss.str().length());
			_font->RenderText(s, 12, y);

			if (_selectedItem == item) {
				// Draw the rectangular marker for selected entry
				selectBoxStartPixel = y - ITEM_HEIGHT / 4;
				glColor3ub(_selectedItem->r, _selectedItem->g, _selectedItem->b);
				float y_ = selectBoxStartPixel;
				string s = _font->TrimHorizontal(_selectedItem->key, 30);
				float w = _font->GetHorizontalPixelLength(s);
				glLineWidth(2);
				glBegin(GL_LINE_STRIP);
				glVertex2f(3, y_);
				glVertex2f(21 + w, y_);
				glVertex2f(21 + w, y_ + ITEM_HEIGHT);
				glVertex2f(3, y_ + ITEM_HEIGHT);
				glVertex2f(3, y_);
				glEnd();
			}
		}
		else {
			_DrawSubLevelItem(item, y);
		}
	}

	// Black out the top and bottom which is not part of the table
	glColor3ub(0, 0, 0);
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

	glColor3ub(255, 255, 255);
	if (_isTopLevelTable) {
	string heading = "Process Names";
	_fontLarge->RenderText(heading, 3, TABLE_TOP + 8);
	}
}

void Table::_DrawSubLevelItem(TableItem *item, float y)
{
	_fontSub->RenderText("Host:  " + item->hostName, 5, y-3);
	_fontSub->RenderText("Pid :  " + item->pid, 5, y-6);
	_fontSub->RenderText("User:  " + item->user, 5, y-9);
	_fontSub->RenderText("Time:  " + item->time, 5, y-12);

//	glColor3ub(0, 0, 255);
//	glLineWidth(2);
//	glBegin(GL_LINE_STRIP);
//	glVertex2f(1, y - (ITEM_HEIGHT - 1));
//	glVertex2f(45, y - (ITEM_HEIGHT - 1));
//	glVertex2f(45, y - 1);
//	glVertex2f(1, y - 1);
//	glVertex2f(1, y - (ITEM_HEIGHT - 1));
//	glEnd();
}

vector<TableItem *> *Table::_GetItemGroup_NoLock(string &itemKey)
{
	for (int i = 0; i < _items.size(); i++)
		if (_items[i][0]->key.compare(itemKey) == 0)
			return &_items[i];
	return NULL;
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

/**
 * Maps the current pixel to start rendering at to an item
 */
int Table::_CurrentPixelToItemIndex()
{
	return _currentPixelIndex / ITEM_HEIGHT;
}

/**
 * Returns the top-left pixel index for the rendering area of an item
 */
float Table::_ItemNumberToPixel(int number)
{
	float offset = fmod(_currentPixelIndex, ITEM_HEIGHT);
	return (TABLE_TOP + offset) - (ITEM_HEIGHT * number);
}

int Table::_RelativePixelToItemIndex(float relativeY)
{
	float offset = fmod(_currentPixelIndex, ITEM_HEIGHT);
	int idx = (TABLE_TOP - relativeY + offset) / ITEM_HEIGHT;
	idx += _CurrentPixelToItemIndex();
	return idx;
}

void Table::_HighlightBoids(vector<TableItem *> group)
{
	for (int i = 0; i < group.size(); i++) {
		TableItem *item = group[i];
		vector<BoidSharedContext *> boids = item->GetBoids();
		for (int j = 0; j < boids.size(); j++) {
			boids[j]->EnableHighlight();
		}
	}
}

void Table::_SortTableAlphabetically()
{
	scoped_lock lock(_mutex);
	sort(_items.begin(), _items.end(), TableGroupCompareAlphabetically());
}

void Table::_SortTableUtilization()
{
	scoped_lock lock(_mutex);
	sort(_items.begin(), _items.end(), TableGroupCompareUtilization());
}

bool Table::_IsSortable()
{
	double tsCurrent = System::GetTimeInSec();
	if (tsCurrent - _tsLastUpdate > 1) {
		_tsLastUpdate = tsCurrent;
		return true;
	}
	return false;
}

