#ifndef TABLE_H_
#define TABLE_H_

#include <list>
#include <vector>
#include <boost/thread/mutex.hpp>
#include "Entity.h"
#include "Font.h"
#include "BoidSharedContext.h"

using namespace std;

class TableItem {
public:
	string key;
	int r, g, b;
	float score;
	string procName, hostName, pid;
	TableItem();
	void AddBoid(BoidSharedContext *subItem);
	vector<BoidSharedContext *> GetBoids();
private:
	boost::mutex _mutex;
	vector<BoidSharedContext *> _boids;
};

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

class Table: public EntityEvent {
public:
	Table(bool isTopLevelTable=true);
	virtual ~Table();
	void Add(TableItem *item);
	void Add(vector<TableItem *> items);
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
	boost::mutex _mutex;
	vector< vector<TableItem *> > _items;
	float _currentPixelIndex; // index of item residing on top of displayed list
	float _selectedPixelIndex; // index of item currently marked and active
	Font *_font, *_fontLarge;
	double _tsLastUpdate;
	Table *_subTable;
	TableItem *_selectedItem;
	bool _isTopLevelTable;
	void _DrawAllItems();
	vector<TableItem *> *_GetItemGroup_NoLock(string &itemKey);
	void _DrawArrows();
	int _CurrentPixelToItemIndex();
	float _ItemNumberToPixel(int number);
	int _RelativePixelToItemIndex(float relativeY);
	void _HighlightBoids(vector<TableItem *> group);
	void _SortTableAlphabetically();
	void _SortTableUtilization();
	bool _IsSortable();
};

#endif /* TABLE_H_ */
