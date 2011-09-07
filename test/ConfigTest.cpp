/*
 * ConfigTest.cpp
 *
 *  Created on: May 26, 2011
 *      Author: arild
 */

#include <gtest/gtest.h>
#include <glog/logging.h>
#include "Config.h"

using namespace std;


class ConfigTest: public ::testing::Test {
protected:
	// You can remove any or all of the following functions if its body
	// is empty.

	ConfigTest()
	{
	}

	virtual ~ConfigTest()
	{
	}

	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:

	virtual void SetUp()
	{
	}

	virtual void TearDown()
	{
	}

};


TEST_F(ConfigTest, FontPath)
{
	const char *path = Config::GetTimesFontPath().c_str();
	LOG(INFO) << "Path: " << path;
	ASSERT_STREQ("/home/arild/lib/times.ttf", path);

}
