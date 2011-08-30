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
	string displayName;
	vector<string> subText;
	int r, g, b;
	float score;
	TableItem(string displayName_, int r_, int g_, int b_);
	void AddRelatedBoid(BoidSharedContext *subItem);
	vector<BoidSharedContext *> GetRelatedBoids();
private:
	boost::mutex _mutex;
	vector<BoidSharedContext *> _relatedBoids;
};

struct TableItemCompare {
	bool operator()(TableItem * const a, TableItem * const b)
	{
		return a->score > b->score;
	}
};

class Table: public EntityEvent {
public:
	Table(bool isTopLevelTable=true);
	virtual ~Table();
	void Add(TableItem *item);

	virtual void OnInit();
	virtual void OnLoop();
	virtual void OnRender();
	virtual void OnCleanup();
	virtual void Tap(float x, float y);
	virtual void ScrollDown(float deltaY);
	virtual void ScrollUp(float deltaY);

private:
	boost::mutex _mutex;
	vector<TableItem *> _items;
	int _currentPixelIndex; // index of item residing on top of displayed list
	int _selectedPixelIndex; // index of item currently marked and active
	Font *_font, *_fontLarge;
	double _tsLastUpdate;
	Table *_subTable;
	bool _isTopLevelTable;
	void _DrawAllItems();
	vector<TableItem *> _GetTopRankedUniqueDisplayNameItems(int numItems);
	bool _HasDisplayName(vector<TableItem *> &items, string &displayName);
	bool _PerformUpdate();
	void _DrawArrows();
};

#endif /* TABLE_H_ */
