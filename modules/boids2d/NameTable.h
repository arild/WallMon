#ifndef TABLE_H_
#define TABLE_H_

#include <list>
#include <vector>
#include <boost/thread/mutex.hpp>
#include "Entity.h"
#include "Font.h"

using namespace std;

class TableItem {
public:
	string displayName;
	int r, g, b;
	double timestampMsec;
	float score;
	//	list<TableItem *> _items;
	TableItem(string displayName_, int r_, int g_, int b_);
};

//typedef list<TableItem *> ItemContainerType;
struct TableItemCompare {
	bool operator()(TableItem * const a, TableItem * const b)
	{
		return a->score > b->score;
	}
};

class NameTable: public EntityEvent {
public:
	NameTable();
	virtual ~NameTable();
	void Add(TableItem *item);

	virtual void OnInit();
	virtual void OnLoop();
	virtual void OnRender();
	virtual void OnCleanup();
	virtual void Up(float x, float y);

private:
	boost::mutex _mutex;
	vector<TableItem *> _itemsAll, _itemsDisplay;
	int _currentPixelIndex; // index of item residing on top of displayed list
	int _selectedPixelIndex; // index of item currently marked and active
	Font *_font;
	Font *_fontLarge;
	double _tsLastUpdate;
	void _DrawAllItems();
	vector<TableItem *> _GetTopRankedUniqueDisplayNameItems(int numItems);
	bool _HasDisplayName(vector<TableItem *> &items, string &displayName);
	bool _PerformUpdate();
};

#endif /* TABLE_H_ */
