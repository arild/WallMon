/*
 * NameListTest.cpp
 *
 *  Created on: May 1, 2011
 *      Author: arild
 */

#include <gtest/gtest.h>
#include "NameTagList.h"

class NameTagListTest: public ::testing::Test {
protected:
	// You can remove any or all of the following functions if its body
	// is empty.

	NameTagList *l;

	NameTagListTest()
	{
		l = new NameTagList();
		for (int i = 0; i < MAX_NUM_NAME_TAGS; i++) {
			NameTag t("a", i, i, i);
			l->Add(t);
		}
	}

	virtual ~NameTagListTest()
	{
		delete l;
	}

	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:

	virtual void SetUp()
	{
		// Code here will be called immediately after the constructor (right
		// before each test).
	}

	virtual void TearDown()
	{
		// Code here will be called immediately after each test (right
		// before the destructor).
	}

};


TEST_F(NameTagListTest, All)
{
	vector<NameTag> v = l->GetList();
	for (int i = 0; i < v.size(); i++) {
		ASSERT_EQ("a", v[i].name);
		ASSERT_EQ(i, v[i].r);
		ASSERT_EQ(i, v[i].g);
		ASSERT_EQ(i, v[i].b);
	}

}
