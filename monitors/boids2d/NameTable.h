
#ifndef TABLE_H_
#define TABLE_H_

#include <list>
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
	bool operator()(TableItem * const  a, TableItem * const b)
	{
		return a->score > b->score;
	}
};

class NameTable : Entity {
public:
	NameTable(int maxNumItemsToDisplay=10);
	virtual ~NameTable();
	void Add(TableItem *item);

	virtual void OnLoop();
	virtual void OnRender();
	virtual void OnCleanup();
	void HandleHit(TouchEvent &event);

private:
	boost::mutex _mutex;
	list<TableItem *> _items;
	Font *_font;
	int _maxNumItemsToDisplay;
	void _DrawAllItems();
};

#endif /* TABLE_H_ */
