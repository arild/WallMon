#ifndef TABLE_H_
#define TABLE_H_

#include <list>
#include <vector>
#include <boost/thread/mutex.hpp>
#include "Queue.h"
#include "Entity.h"
#include "Font.h"
#include "TableItem.h"
#include "BoidSharedContext.h"

using namespace std;

class Table: public EntityEvent {
public:
	Table(bool isTopLevelTable=true);
	virtual ~Table();

	void Add(TableItem *item);
	void Add(vector<TableItem *> items);
	void Remove(TableItem *item);
	void Clear();

	virtual void OnInit();
	virtual void OnLoop();
	virtual void OnRender();
	virtual void OnCleanup();
	virtual void Tap(float x, float y);
	virtual void ScrollDown(float speed);
	virtual void ScrollUp(float speed);
	virtual void SwipeLeft(float speed);
	virtual void SwipeRight(float speed);

private:
	vector< vector<TableItem *> > _items;
	float _currentPixelIndex; // index of item residing on top of displayed list
	float _itemHeight;
	Font _fontSub, _fontSubLarge, _font;
	Table *_subTable;
	TableItem *_selectedItem;
	Queue<TableItem *> _addQueue, _removeQueue;
	bool _isHighlighted, _isTopLevelTable;
	int _processTerminationIndex;
	void _DrawTopLevelTable();
	void _DrawSubLevelTable();
	void _DrawBlackBorders();
	void _DrawCommonHeadingAndFrame();
	void _DrawSubLevelItem(TableItem *item, float y, int highlightNumber);
	int _CurrentPixelToItemIndex();
	float _ItemNumberToPixel(int number);
	int _RelativePixelToItemIndex(float relativeY);
	void _HighlightBoids(vector<TableItem *> group);
	void _UnHighlightBoids(vector<TableItem *> group);
	void _SortTableAlphabetically();
	void _SortTableScore();
	bool _IsSortable();
	vector<TableItem *> *_LookupItemGroup(string &itemKey);
	void _TerminateProcess(TableItem &item);
};

#endif /* TABLE_H_ */
