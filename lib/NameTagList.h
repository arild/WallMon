/*
 * NameList.h
 *
 *  Created on: Apr 29, 2011
 *      Author: arild
 */

#ifndef NAMELIST_H_
#define NAMELIST_H_

#define MAX_NUM_NAME_TAGS		20

#include <vector>
#include <map>
#include <boost/thread/mutex.hpp>

using namespace std;

typedef boost::mutex::scoped_lock scoped_lock;

class NameTag {
public:
	string name;
	int r, g, b;
	NameTag(string _name, int _r, int _g, int _b);
};

class NameTagList {
public:
	NameTagList();
	virtual ~NameTagList();
	void Add(NameTag &item);
	vector<NameTag> GetList();

private:
	boost::mutex _mutex;
	vector<NameTag> _nameItems;
	bool _HasName(string name);
};

#endif /* NAMELIST_H_ */
