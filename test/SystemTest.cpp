#include <gtest/gtest.h>
#include "System.h"

using namespace std;

// The fixture for testing class Foo.
class SystemTest: public ::testing::Test {
protected:
	// You can remove any or all of the following functions if its body
	// is empty.

	SystemTest()
	{
	}

	virtual ~SystemTest()
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


TEST_F(SystemTest, NumCores)
{
	ASSERT_EQ(4, System::GetNumCores());
}

TEST_F(SystemTest, CurrentUser)
{
	ASSERT_EQ("arild", System::GetCurrentUser());
}
