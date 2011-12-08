/**
 * Implements a scrollable and touchable table that holds and displays
 * named entries. The implementation is somewhat general, however, tailored
 * for having process names in a higher-level table, while specific processes
 * in a lower-level table. The implementation is based on having two instances
 * of the table object with the same set of methods, where differences are accounted
 * for via branching on their identity, similar to patterns seen in MPI.
 *
 * The implementation accesses the list of items without any locking mechanisms because:
 * - Touch events and render callbacks are carried out sequentially by the main loop
 * - External addition and deletion of table entries are temporarily stored in (thread-safe)
 *   queues, before being processed in render callbacks
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

float TABLE_TOP = 85.;
float TABLE_BOTTOM = 15.;
float FONT_SIZE = 3.;
Table *topTable = NULL;
ControlPanel *Table::controlPanel = NULL;

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
		float aScore = 0;
		float bScore = 0;

		// Calculate score sum
		int len = max(a.size(), b.size());
		for (int i = 0; i < len; i++) {
			if (i < a.size())
				aScore += a[i]->GetScore();
			if (i < b.size())
				bScore += b[i]->GetScore();
		}
		// Calculate score average
		aScore /= (float)a.size();
		bScore /= (float)b.size();

		// Favor process groups with many members
		aScore += aScore * 0.03 * a.size();
		bScore += bScore * 0.03 * b.size();

		return aScore > bScore;
	}
};


Table::Table(bool isMaster, bool isTopLevelTable)
{
	_isTopLevelTable = isTopLevelTable;
	_currentPixelIndex = 0;
	_selectedItem = NULL;
	_selectedItemIndex = -1;
	_addQueue = new Queue<TableItem *>(25000);
	_removeQueue = new Queue<TableItem *>(300);
	// Set up hit box
	if (_isTopLevelTable) {
		topTable = this;
		tx = 0;
		ty = TABLE_BOTTOM;
		width = 50;
		height = TABLE_TOP;
		_state = new TableStateSynchronizer();
		_state->Start();


		// Create and setup hitbox for the sub-table
		// Note: Entities created by other entities are automatically
		// added to current scene
		_subTable = new Table(isMaster, false);
		_subTable->SetTableStateSynchronizer(_state);
		Scene::AddEntityCurrent(_subTable);
		_subTable->tx = 50;
		_subTable->ty = 0;
		_subTable->width = 50;
		_subTable->height = 100;

		_itemHeight = 6;
	}
	else {
		_itemHeight = 15;
		_isHighlighted = false;
		_processTerminationIndex = -1;
	}
	_isMaster = isMaster;
	_font.SetFontSize(FONT_SIZE);
	_fontSub.SetFontSize(FONT_SIZE - 1);
	_fontSub.SetFontType(FONT_MONO);
	_fontSub.SetAlignmentPolicy(false, true);
	_fontSubLarge.SetFontSize(FONT_SIZE);
	_fontSubLarge.SetFontType(FONT_MONO);
	SetSwipeEventInterval(0.75);
	SetTapEventInterval(1.1);
}

Table::~Table()
{
	delete _subTable;
}

void Table::SetTableStateSynchronizer(TableStateSynchronizer *state)
{
	_state = state;
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
	_addQueue->Push(item);
}

void Table::Add(vector<TableItem *> items)
{
	for (int i = 0; i < items.size(); i++)
		Add(items[i]);
}

void Table::Remove(TableItem *item)
{
	_removeQueue->Push(item);
}

void Table::Clear()
{
	_removeQueue->Push((TableItem *)NULL);
}

void Table::OnInit()
{
}

void Table::OnLoop()
{
	// Process state messages
	Queue<TableStateMessage> *stateQueue = _state->GetStateMessageQueue(_isTopLevelTable);
	while (stateQueue->GetSize() > 0) {
		TableStateMessage msg = stateQueue->Pop();
		_currentPixelIndex = msg.pixelindex();
		if (msg.has_selectedindex()) {
			int idx = msg.selectedindex();
			if (_isTopLevelTable) {
				if (idx < 0 || idx >= _items.size())
					return;
				TableItem *selectedItem = _items[idx][0];
				// Populate the sub-table with all items in selected group
				vector<TableItem *> group = _items[idx];
				_subTable->Clear();
				if (idx == _selectedItemIndex) {
					// Unhighlight
					_selectedItemIndex = -1;
				}
				else {
					_selectedItemIndex = idx;
					_subTable->Add(group);
				}
			}
			else {
				if (_items.size() == 0 || idx < 0 || idx >= _items[0].size())
					return;
				//if (idx == _processTerminationIndex)
					// Second tap
					_TerminateProcess(*_items[0][idx]);
				_processTerminationIndex = idx;
			}
		}
		if (msg.has_swipeleft() && _isTopLevelTable)
			_SortTableAlphabetically();
		if (msg.has_swiperight()) {
			if (_isTopLevelTable)
				_SortTableScore();
			else {
				if (_items.size() == 0)
					// No items present in sub-table (tap not performed)
					return;
				if (_isHighlighted) {
					_UnHighlightBoids(_items[0]);
					_isHighlighted = false;
				}
				else {
					_HighlightBoids(_items[0]);
					_isHighlighted = true;
				}
			}
		}

	}

	// Process removal of single or all items
	while (_removeQueue->GetSize() > 0) {
		TableItem *item = _removeQueue->Pop();
		if (item == NULL) {
			// Clear all items
			for (int i = 0; i < _items.size(); i++) {
				_UnHighlightBoids(_items[i]);
			}
			_items.clear();
			_currentPixelIndex = 0;
			_selectedItemIndex = -1;
			_processTerminationIndex = -1;
		}
		else {
			// Search through the two-dimensional space of table items.
			// If the particular table item is found, it is removed
			for (int i = 0; i < _items.size(); i++)
				for (int j = 0; j < _items[i].size(); j++)
					if (item == _items[i][j]) {
						_items[i].erase(_items[i].begin() + j);
						if (_items[i].size() == 0)
							// No more items in group
							_items.erase(_items.begin() + i);
					}
		}

	}

	// Handle addition of new items
	while (_addQueue->GetSize() > 0) {
		TableItem *item = _addQueue->Pop();
		vector<TableItem *> *itemGroup = _LookupItemGroup(item->key);
		if (itemGroup == NULL) {
			// Group not present, create it
			vector<TableItem *> v;
			v.push_back(item);
			_items.push_back(v);
		} else
			itemGroup->push_back(item);
	}

}

void Table::OnRender()
{
	if (_isTopLevelTable) {
		_DrawTopLevelTable();
	}
	else {
		glTranslatef(50, 0, 0);
		_DrawSubLevelTable();
		_DrawCommonHeadingAndFrame();
	}
}

void Table::OnCleanup()
{
	delete _addQueue;
	delete _removeQueue;
}

void Table::Tap(float x, float y)
{
	if (!_isMaster)
		return;

	if (y > TABLE_TOP || y < TABLE_BOTTOM)
		return;
	int idx = _RelativePixelToItemIndex(y);
	_SynchronizeState(_currentPixelIndex, idx);
}

void Table::ScrollDown(float speed)
{
	if (!_isMaster)
		return;
	float currentPixelIndex = _currentPixelIndex + speed;
	int numItems = _items.size();
	if (!_isTopLevelTable) {
		if (_items.size() == 0)
			// No items present in sub-table (tap not performed)
			return;
		numItems = _items[0].size();
	}
	// Allow half an item to fade on top
	numItems = max(numItems, 2);
	int maxPixelIndex = (numItems * _itemHeight) - _itemHeight*(float)1.5;
	currentPixelIndex = fmin(currentPixelIndex, (float)maxPixelIndex);
	_SynchronizeState(currentPixelIndex);
}

void Table::ScrollUp(float speed)
{
	if (!_isMaster)
		return;
	float currentPixelIndex = _currentPixelIndex - speed;
	currentPixelIndex = fmax(currentPixelIndex, -15);
	_SynchronizeState(currentPixelIndex);
}

void Table::SwipeLeft(float speed)
{
	if (!_isMaster)
		return;
	_SynchronizeState(_currentPixelIndex, -1, speed, -1);
}

void Table::SwipeRight(float speed)
{
	if (!_isMaster)
		return;
	_SynchronizeState(_currentPixelIndex, -1, -1, speed);
}

void Table::Clap()
{
	controlPanel->Clap();
}

void Table::_DrawTopLevelTable()
{
	float selectBoxStartPixel;
	int startIndex = _CurrentPixelToItemIndex();
	for (int i = max(startIndex, 0); i < _items.size() ; i++) {

		float y = _ItemNumberToPixel(i - startIndex);
		if (y < 0)
			break;
		if (_items[i].size() == 0)
			LOG(INFO) << "group is empty";

		// Render first item of group. For the sub-table, there is only
		// groups containing one item, hence all items in the sub-table are rendered
		TableItem *item = _items[i][0];
		int groupSize = _items[i].size();

		// Draw the rectangle in front of every entry
		glColor3ub(item->r, item->g, item->b);
		y -= 4.5;
		glBegin(GL_QUADS);
		glVertex2f(6, y);
		glVertex2f(9, y);
		glVertex2f(9, y + 3);
		glVertex2f(6, y + 3);
		glEnd();

		stringstream ss, ss2;
		ss << groupSize;
		ss2 << item->_procName << " (" << ss.str() << ")";
		string s = _font.TrimHorizontal(ss2.str(), 32, 4 +  ss.str().length());
		_font.RenderText(s, 12, y);

		if (_selectedItemIndex == i) {
			// Draw the rectangular marker for selected entry
			selectBoxStartPixel = y - _itemHeight / 4;
			glColor3ub(item->r, item->g, item->b);
			float y_ = selectBoxStartPixel;
			string s = _font.TrimHorizontal(item->key, 30);
			float w = _font.GetHorizontalPixelLength(s);
			glLineWidth(2);
			glBegin(GL_LINE_STRIP);
			glVertex2f(3, y_);
			glVertex2f(21 + w, y_);
			glVertex2f(21 + w, y_ + _itemHeight);
			glVertex2f(3, y_ + _itemHeight);
			glVertex2f(3, y_);
			glEnd();
		}
	}
	_DrawBlackBorders();
	glColor3ub(255, 255, 255);
}

void Table::_DrawSubLevelTable()
{
	if (_items.size() == 0)
		return;
	int startIndex = _CurrentPixelToItemIndex();
	vector<TableItem *> group = _items[0];
	glColor3ub(group[0]->r, group[0]->g, group[0]->b);

	for (int i = max(startIndex, 0); i < group.size(); i++) {
		float y = _ItemNumberToPixel(i - startIndex);
		if (y < 0)
			break;
		_DrawSubLevelItem(group[i], y, group[i]->highlightNumber);
	}
	_DrawBlackBorders();
}

void Table::_DrawBlackBorders()
{
	// Black out the top and bottom which is not part of the table
	glColor3ub(0, 0, 0);
	glBegin(GL_QUADS);
	glVertex2f(-20, TABLE_TOP);
	glVertex2f(50, TABLE_TOP);
	glVertex2f(50, 120);
	glVertex2f(-20, 120);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(-20, -20);
	glVertex2f(50, -20);
	glVertex2f(50, TABLE_BOTTOM);
	glVertex2f(-20, TABLE_BOTTOM);
	glEnd();
}

void Table::_DrawCommonHeadingAndFrame()
{
	glColor3ub(255, 255, 255);
	_font.RenderText("Process Explorer", 0, TABLE_TOP + 9, true, false);
	glColor3f(1, 0, 0);
	stringstream ss;
	ss << "monitoring " << topTable->_GetTotalNumItems() << " processes";
	_fontSub.RenderText(ss.str(), 0, TABLE_TOP + 5, true, false);
}

void Table::_DrawSubLevelItem(TableItem *item, float y, int highlightNumber)
{
	// Note: Getters on item not used due to some strange bug causing
	// a crash when copying variables (only for strings have been observed)
	glColor3ub(item->r, item->g, item->b);
	_fontSub.RenderText("Host:  " + item->_hostName, 5, y-2.5);
	_fontSub.RenderText("Pid :  " + item->_pid, 5, y-5);
	_fontSub.RenderText("User:  " + item->_user, 5, y-7.5);
	_fontSub.RenderText("Time:  " + item->_time, 5, y-10);
	_fontSub.RenderText("Threads:  " + item->_numThreads, 5, y-12.5);
	if (highlightNumber != -1) {
		glColor3ub(255, 0, 0);
		stringstream ss;
		ss << highlightNumber;
		_fontSubLarge.RenderText(ss.str(), 1, y - _itemHeight/(float)2, true, true);
	}
}

/**
 * Maps the current pixel to start rendering at to an item
 */
int Table::_CurrentPixelToItemIndex()
{
	return _currentPixelIndex / _itemHeight;
}

/**
 * Returns the top-left pixel index for the rendering area of an item
 */
float Table::_ItemNumberToPixel(int number)
{
	float offset = fmod(_currentPixelIndex, _itemHeight);
	return (TABLE_TOP + offset) - (_itemHeight * number);
}

int Table::_RelativePixelToItemIndex(float relativeY)
{
	float offset = fmod(_currentPixelIndex, _itemHeight);
	int idx = (TABLE_TOP - relativeY + offset) / _itemHeight;
	idx += _CurrentPixelToItemIndex();
	return idx;
}

void Table::_HighlightBoids(vector<TableItem *> group)
{
	for (int i = 0; i < group.size(); i++) {
		TableItem *item = group[i];
		item->highlightNumber = i;
		vector<BoidSharedContext *> boids = item->GetBoids();
		for (int j = 0; j < boids.size(); j++) {
			boids[j]->EnableHighlight(i);
		}
	}
}

void Table::_UnHighlightBoids(vector<TableItem *> group)
{
	for (int i = 0; i < group.size(); i++) {
		TableItem *item = group[i];
		item->highlightNumber = -1;
		vector<BoidSharedContext *> boids = item->GetBoids();
		for (int j = 0; j < boids.size(); j++) {
			boids[j]->DisableHighlight();
		}
	}
}

void Table::_SortTableAlphabetically()
{
	// Preserve the item visually selected
	TableItem *selected;
	if (_selectedItemIndex > -1)
		selected = _items[_selectedItemIndex][0];
	sort(_items.begin(), _items.end(), TableGroupCompareAlphabetically());
	if (_selectedItemIndex > -1)
		_selectedItemIndex = _TableItemToIndex(selected);
}

void Table::_SortTableScore()
{
	// Preserve the item visually selected
	TableItem *selected;
	if (_selectedItemIndex > -1)
		selected = _items[_selectedItemIndex][0];
	sort(_items.begin(), _items.end(), TableGroupCompareUtilization());
	if (_selectedItemIndex > -1)
		_selectedItemIndex = _TableItemToIndex(selected);
}

vector<TableItem *> *Table::_LookupItemGroup(string &itemKey)
{
	for (int i = 0; i < _items.size(); i++)
		if (_items[i][0]->key.compare(itemKey) == 0)
			return &_items[i];
	return NULL;
}

void Table::_TerminateProcess(TableItem &item)
{
	stringstream ss;
	ss << "ssh " << item.GetHostName() << " \"kill -9 " << item.GetPid() << "\"";
	System::RunCommand(ss.str());
}

int Table::_GetTotalNumItems()
{
	int num = 0;
	for (int i = 0; i < _items.size(); i++) {
		num += _items[i].size();
	}
	return num;
}

void Table::_SynchronizeState(float pixelIndex, int selectedIndex, float swipeLeft, float swipeRight)
{
	TableStateMessage msg = GetStateMessage();
	msg.set_pixelindex(pixelIndex);
	if (selectedIndex >= 0)
		msg.set_selectedindex(selectedIndex);
	if (swipeLeft > 0)
		msg.set_swipeleft(swipeLeft);
	if (swipeRight > 0)
		msg.set_swiperight(swipeRight);
	_state->SynchronizeState(msg);
}

TableStateMessage Table::GetStateMessage()
{
	TableStateMessage msg;
	msg.set_istoptable(_isTopLevelTable);
	return msg;
}

int Table::_TableItemToIndex(TableItem *item)
{
	for (int i = 0; i < _items.size(); i++)
		if (item == _items[i][0])
			return i;
	return 0;
}
