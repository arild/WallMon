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
	vector<string> subText;
	int r, g, b;
	float score;
	TableItem(string displayName_, int r_, int g_, int b_);
	void AddBoid(BoidSharedContext *subItem);
	vector<BoidSharedContext *> GetBoids();
private:
	boost::mutex _mutex;
	vector<BoidSharedContext *> _subItems;
};

struct TableGroupCompare {
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
	virtual void ScrollDown(float deltaY);
	virtual void ScrollUp(float deltaY);

private:
	boost::mutex _mutex;
	vector< vector<TableItem *> > _items;
	int _currentPixelIndex; // index of item residing on top of displayed list
	int _selectedPixelIndex; // index of item currently marked and active
	Font *_font, *_fontLarge;
	double _tsLastUpdate;
	Table *_subTable;
	bool _isTopLevelTable;
	void _DrawAllItems();
	vector<TableItem *> *_GetItemGroup_NoLock(string &itemKey);
	bool _PerformUpdate();
	void _DrawArrows();
	int _SelectedPixelToItemIndex();
	int _CurrentPixelToItemIndex();
	int _GetStartPixel();
};

#endif /* TABLE_H_ */
