/*
 * WallViewTest.cpp
 *
 *  Created on: Apr 28, 2011
 *      Author: arild
 */

#include <gtest/gtest.h>
#include <vector>
#include "WallView.h"
#include "System.h"

using namespace std;

class WallViewTest: public ::testing::Test {
protected:
	WallViewTest()
	{
	}

	virtual ~WallViewTest()
	{
	}

	virtual void SetUp()
	{
	}

	virtual void TearDown()
	{
	}

};


TEST_F(WallViewTest, NotMemberOfDisplayWall)
{
	ASSERT_FALSE(WallView(0, 0, 7, 4).IsWithin());
}

TEST_F(WallViewTest, OutOfBounds)
{
	WallView w = WallView(0, 0, 99, 77);
	ASSERT_FALSE(w.IsWithin());

	w = WallView(1, 3, 99, 77);
	ASSERT_FALSE(w.IsWithin());
}

TEST_F(WallViewTest, CorrectTileNames)
{
	WallView w = WallView(0, 0, 2, 2);
	vector<string> v = w.Get();
	ASSERT_EQ(v.size(), 4);
	ASSERT_EQ(0, v[0].compare("tile-0-0"));
	ASSERT_EQ(0, v[1].compare("tile-1-0"));
	ASSERT_EQ(0, v[2].compare("tile-0-1"));
	ASSERT_EQ(0, v[3].compare("tile-1-1"));
}

TEST_F(WallViewTest, Orientation)
{
	WallView view = WallView(0, 0, 4, 2);
	vector<string> v = view.Get();
	ASSERT_EQ(8, v.size());
	double x, y, w, h;

	view.GetOrientation(&x, &y, &w, &h, "tile-0-0");
	ASSERT_EQ(0, x);
	ASSERT_EQ(0, y);
	ASSERT_EQ(25, w);
	ASSERT_EQ(50, h);

	view.GetOrientation(&x, &y, &w, &h, "tile-2-1");
	ASSERT_EQ(50, x);
	ASSERT_EQ(50, y);
	ASSERT_EQ(25, w);
	ASSERT_EQ(50, h);

	view = WallView(0, 0, 2, 2);
	v = view.Get();

	view.GetOrientation(&x, &y, &w, &h, "tile-0-0");
	ASSERT_EQ(0, x);
	ASSERT_EQ(0, y);
	ASSERT_EQ(50, w);
	ASSERT_EQ(50, h);

	view.GetOrientation(&x, &y, &w, &h, "tile-0-1");
	ASSERT_EQ(0, x);
	ASSERT_EQ(50, y);
	ASSERT_EQ(50, w);
	ASSERT_EQ(50, h);

}



