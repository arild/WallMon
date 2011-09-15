#include <gtest/gtest.h>
#include <glog/logging.h>
#include "Ps.h"
#include "System.h"

using namespace std;

class PsTest: public ::testing::Test {
protected:
	Ps ps;
	PsTest()
	{
	}

	virtual ~PsTest()
	{
	}

	virtual void SetUp()
	{
	}

	virtual void TearDown()
	{
	}
};

TEST_F(PsTest, All)
{
	int pid = atoi(System::RunCommand("pidof opera").c_str());
	LOG(INFO) << "pid: " << pid;
	ASSERT_EQ("arild", ps.PidToUser(pid));
}
