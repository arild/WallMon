#include <sstream>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include "LinuxProcessMonitorLight.h"
#include "System.h"

using namespace std;

class LinuxProcessMonitorLightTest: public ::testing::Test {
protected:
	LinuxProcessMonitorLightTest()
	{
	}

	virtual ~LinuxProcessMonitorLightTest()
	{
	}

	virtual void SetUp()
	{
	}

	virtual void TearDown()
	{
	}
};

TEST_F(LinuxProcessMonitorLightTest, All)
{
//	System::RunCommand("top &");
	return;
	int pid = atoi(System::RunCommand("pidof top").c_str());
	LinuxProcessMonitorLight m;
	ASSERT_TRUE(m.Open(pid));
	ASSERT_TRUE(m.Update());
	stringstream ss;
	ss << "kill " << pid;
	System::RunCommand(ss.str());
	sleep(3);
	ASSERT_FALSE(m.Update());
}
