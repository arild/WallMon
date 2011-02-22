#include <map>
#include <gtest/gtest.h>

using namespace std;
typedef map<string, int> mymap;

// The fixture for testing class Foo.
class MapTest: public ::testing::Test {
protected:
	// You can remove any or all of the following functions if its body
	// is empty.

	MapTest()
	{
	}

	virtual ~MapTest()
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

	mymap _m;
};


TEST_F(MapTest, Basic)
{
	mymap *m = new mymap();
	(*m)["A"] = 2;
	ASSERT_EQ(2, (*m)["A"]);
}

TEST_F(MapTest, Stack)
{
	_m["A"] = 2;
	ASSERT_EQ(2, _m["A"]);
}
