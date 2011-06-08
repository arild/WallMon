
#include <GL/gl.h>
#include <boost/foreach.hpp>
#include <glog/logging.h>
#include <sstream>
#include "System.h"
#include "NameTable.h"


typedef boost::mutex::scoped_lock scoped_lock;


TableItem::TableItem(string displayName_, int r_, int g_, int b_)
{
	displayName = displayName_;
	r = r_;
	g = g_;
	b = b_;
	timestampMsec = System::GetTimeInMsec();
	score = 0;
}

NameTable::NameTable(int maxNumItemsToDisplay)
{
	_font = new Font(5);
	_maxNumItemsToDisplay = maxNumItemsToDisplay;
}


NameTable::~NameTable()
{
}

void NameTable::Add(TableItem *item)
{
	LOG(INFO) << "Addind item: " << item->displayName;
	scoped_lock lock(_mutex);
//	if (_items.)
	_items.insert(item);
}

void NameTable::OnLoop()
{
	// Eviction policy
}

void NameTable::OnRender()
{
	scoped_lock lock(_mutex);

	glColor3ub(1, 0, 0);
	stringstream ss;
	ss << "Num Entries: " << _items.size();
	_font->RenderText(ss.str(), 5, 95);
	_font->RenderText("WTF", 50, 50);
	_DrawAllItems();
}

void NameTable::OnCleanup()
{

}

void NameTable::HandleHit(TouchEvent & event)
{
}

void NameTable::_DrawAllItems()
{
	float s = 3;
	float y = 90;

	BOOST_FOREACH(TableItem *item, _items) {
		y -= s;

		glColor3ub(item->r, item->g, item->b);
		glBegin(GL_QUADS);
		glVertex2f(0, y);
		glVertex2f(s, y);
		glVertex2f(s, y+s);
		glVertex2f(0, y+s);
		glEnd();

		_font->RenderText(item->displayName, s*2, y+s);

		y -= s;

		stringstream ss;
		ss << "Score: " << item->score;
		_font->RenderText(ss.str(), s*2, y+s);

		y -= s;
	}
}







